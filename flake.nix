{
  inputs = {
    nixpkgs = {};
  };
  outputs = inputs@{ self, nixpkgs }:
    let
      inherit (nixpkgs) lib;
      sdl3-gfx = nixpkgs: nixpkgs.stdenv.mkDerivation {
        pname = "sdl3-gfx";
        version = "1.0.0-unstable-2025";
        src = nixpkgs.fetchFromGitHub {
          owner = "sabdul-khabir";
          repo = "SDL3_gfx";
          rev = "0bbee988bb0caa3e98a9d78c7a2d106925c8275a";
          hash = "sha256-uHyCXYTv8D2DzuLSyIsgYfWgtrCdC5UiZEYhUdFzNOk=";
        };
        nativeBuildInputs = [ nixpkgs.cmake nixpkgs.pkg-config ];
        buildInputs = [ nixpkgs.sdl3 ];
      };
      grobotsPkg = { stdenv, lib, meson, ninja, pkg-config, withSDL ? true
        , sdl3 ? null, sdl3-ttf ? null, sdl3-gfx ? null, windows ? null, withWine ? false, wine }:
        stdenv.mkDerivation {
          pname = "grobots";
          version = builtins.substring 0 8 self.lastModifiedDate;
          src = ./.;
          nativeBuildInputs = [ meson ninja ]
                              ++ (lib.optionals (withSDL || withWine) [ pkg-config ]);
          buildInputs = (lib.optionals withSDL [ sdl3 sdl3-ttf sdl3-gfx ]) ++ (lib.optionals withWine [ wine ]);
          postInstall = lib.optionalString (stdenv.system == "i686-windows") ''
            cp ${windows.mcfgthreads}/bin/*.dll $out/bin
            cp ${stdenv.cc.cc.out}/i686-w64-mingw32/lib/*.dll $out/bin
          '';
        };
      forEachSystem = lib.genAttrs [ "x86_64-linux" "i686-linux" ];
      portsF = fetchurl: {
        freetype = fetchurl {
          url = "https://github.com/freetype/freetype/archive/VER-2-13-3.zip";
          hash = "sha256-pxZctAXfzjSa5k1vWe+yVnVlyo7brL7/Ko0OTPp3nPU=";
        };
        sdl3 = fetchurl {
          url = "https://github.com/libsdl-org/SDL/archive/release-3.2.4.zip";
          hash = "sha256-ijLJIC7U+z1VYDkVL1rR+IzdAKncD5Zx3fDN1gmUBZI=";
        };
        harfbuzz = fetchurl {
          url = "https://github.com/harfbuzz/harfbuzz/releases/download/3.2.0/harfbuzz-3.2.0.tar.xz";
          hash = "sha256-CtpQocGZu29whDq4k8VYZ3Q6RDuE0IfVTfCK2IPrws0=";
        };
        sdl3_ttf = fetchurl {
          url = "https://github.com/libsdl-org/SDL_ttf/archive/release-3.2.2.zip";
          hash = "sha256-CjvBQm7hb8CWnpbxK675/z2YC+YbVMqlT6tnmDvOcYY=";
        };
        sdl3_gfx = fetchurl {
          url = "https://github.com/sabdul-khabir/SDL3_gfx/archive/0bbee988bb0caa3e98a9d78c7a2d106925c8275a.zip";
          hash = "sha256-IqwxV/4edvmd/pITSVavLeRFFxCCWYhft4G01E8axJU=";
        };
        zlib = fetchurl {
          url = "https://github.com/madler/zlib/archive/refs/tags/v1.3.1.tar.gz";
          hash = "sha256-F+iIY/NgBnKrSRgvIXKBtvxNPHYr3jYZNeQ2qVIU0Fw=";
        };

      };
    in {
      packages = forEachSystem (system:
        let nixpkgs = inputs.nixpkgs.legacyPackages.${system};
            ports = portsF nixpkgs.fetchurl;
        in {
          default = nixpkgs.callPackage grobotsPkg { withSDL = true; sdl3-gfx = sdl3-gfx nixpkgs; };
          emcc = (nixpkgs.callPackage grobotsPkg {
            withSDL = false;
          }).overrideAttrs (o: {
            mesonFlags = (o.mesonFlags or []) ++ ["--cross-file" "wasm.txt"];
            nativeBuildInputs = o.nativeBuildInputs ++ [ nixpkgs.emscripten nixpkgs.libarchive ];
            EM_CACHE = "/tmp/emcc-cache";
            EM_PORTS = "/tmp/ports";
            postUnpack = lib.concatStringsSep "\n" (lib.mapAttrsToList (name: fetched:
              let
                urlFilename = lib.last (lib.splitString "/" fetched.url);
                ext = lib.concatStringsSep "." (lib.tail (lib.splitString "." urlFilename));
              in ''
              mkdir -p $EM_PORTS/${name}/
              ln -s ${fetched} $EM_PORTS/${name}.${ext}
              bsdtar -x -f $EM_PORTS/${name}.${ext} -C $EM_PORTS/${name}/
              echo ${fetched.url} > $EM_PORTS/${name}/.emscripten_url
            '') ports);
            postInstall = ''
              cp grobots_wasm.{js,wasm} $out/bin/
            '';
          });
          wine = let
            wine = if system == "x86_64-linux" then nixpkgs.wine64 else inputs.nixpkgs.legacyPackages.x86_64-linux.wine;
          in (nixpkgs.callPackage grobotsPkg {
            withSDL = false;
            withWine = true;
            inherit wine;
          }).overrideAttrs (o: {
            buildInputs = o.buildInputs ++ [ nixpkgs.makeWrapper ];
            WINELOADER = if nixpkgs.buildPlatform.is64bit then "${wine}/bin/wine64" else "${wine}/bin/wine";
            shellHook = ''
              export CC=winegcc
              export CXX=wineg++
            '';
            preConfigure = ''
              export WINEPREFIX=/tmp/.wine
              mkdir /tmp/.wine
              chown $UID /tmp/.wine
              $shellHook
            '';
            installPhase = ''
              mkdir -p $out/bin
              cp *.exe.so $out/bin
              makeWrapper $WINELOADER $out/bin/grobots --add-flags $out/bin/grobots.exe.so
              makeWrapper $WINELOADER $out/bin/grobots_headless --add-flags $out/bin/grobots_headless.exe.so
            '';
          });
          mingw32 = nixpkgs.pkgsCross.mingw32.callPackage grobotsPkg {
            withSDL = false;
          };
        });

      devShells = forEachSystem (system:
        let
          nixpkgs = inputs.nixpkgs.legacyPackages.${system};
          pkgs = self.packages.${system};
        in {
          default = pkgs.default.overrideAttrs (o: {
            nativeBuildInputs = o.nativeBuildInputs ++ [ nixpkgs.clang-tools ];
          });
          #inherit (pkgs) default;
          emcc = nixpkgs.mkShell {
            shellHook = ''
              export EM_PORTS=$PWD/cache
              export EM_CACHE=$PWD/cache
            '';
            inputsFrom = [ pkgs.default ];
            nativeBuildInputs = [ nixpkgs.emscripten ];
          };
          mingw32 = pkgs.mingw32.overrideAttrs (o: {
            shellHook = ''
              echo 'meson build $mesonFlags'
            '';
            nativeBuildInputs = o.nativeBuildInputs
              ++ (with nixpkgs; [ wine binutils-unwrapped ]);
          });
        });
    };
}
