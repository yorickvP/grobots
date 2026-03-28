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
          url = "https://github.com/emscripten-ports/FreeType/archive/version_1.zip";
          hash = "sha256-DN/XBFFyq/5EMobrx5X6lrzrrZRxzjSZww8bu1Rypt8=";
        };
        sdl2 = fetchurl {
          url = "https://github.com/libsdl-org/SDL/archive/release-2.24.0.zip";
          hash = "sha256-AkCG2l0+JMN/Q9unBsM7r161+9zm8NNod9mPaweX9I8=";
        };
        harfbuzz = fetchurl {
          url = "https://storage.googleapis.com/webassembly/emscripten-ports/harfbuzz-3.2.0.tar.gz";
          hash = "sha256-zleu2MfDtL459SKFWSjNqtgosFRubU/qjv98+Mp0dN4=";
        };
        sdl2_ttf = fetchurl {
          url = "https://github.com/libsdl-org/SDL_ttf/archive/38fcb695276ed794f879d5d9c5ef4e5286a5200d.zip";
          hash = "sha256-31O0aMTje+VCSwjhEbS4JL2+MZ+B4Orstg6KHmaZzhE=";
        };
        sdl2_gfx = fetchurl {
          url = "https://github.com/svn2github/sdl2_gfx/archive/2b147ffef10ec541d3eace326eafe11a54e635f8.zip";
          hash = "sha256-wRGRoySzrOcDdCd7JjOwMqhVmwrxaKjN5D/9oGg3Ve0=";
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
            postUnpack = lib.concatStringsSep "\n" (lib.mapAttrsToList (name: fetched: ''
              mkdir -p $EM_PORTS/${name}/
              ln -s ${fetched} $EM_PORTS/${name}.zip
              bsdtar -x -f $EM_PORTS/${name}.zip -C $EM_PORTS/${name}/
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
