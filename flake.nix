{
  outputs = inputs@{ self, nixpkgs }:
    let
      grobotsPkg = { stdenv, lib, meson, ninja, pkg-config, withSDL ? true
        , SDL2 ? null, SDL2_ttf ? null, SDL2_gfx ? null, windows ? null, withWine ? false, wine }:
        stdenv.mkDerivation {
          pname = "grobots";
          version = builtins.substring 0 8 self.lastModifiedDate;
          src = ./.;
          nativeBuildInputs = [ meson ninja ]
                              ++ (lib.optionals (withSDL || withWine) [ pkg-config ]);
          buildInputs = (lib.optionals withSDL [ SDL2 SDL2_ttf SDL2_gfx ]) ++ (lib.optionals withWine [ wine ]);
          postInstall = lib.optionalString (stdenv.system == "i686-windows") ''
            cp ${windows.mcfgthreads}/bin/*.dll $out/bin
            cp ${stdenv.cc.cc.out}/i686-w64-mingw32/lib/*.dll $out/bin
          '';
        };
      forEachSystem = nixpkgs.lib.genAttrs [ "x86_64-linux" "i686-linux" ];
    in {
      packages = forEachSystem (system:
        let nixpkgs = inputs.nixpkgs.legacyPackages.${system};
        in {
          default = nixpkgs.callPackage grobotsPkg { withSDL = true; };
          wine = (nixpkgs.callPackage grobotsPkg { withSDL = false; withWine = true; wine = inputs.nixpkgs.legacyPackages.x86_64-linux.wine; }).overrideAttrs (o: {
            # WINELOADER = "wine64";
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
          inherit (pkgs) default;
          emcc = nixpkgs.mkShell {
            shellHook = ''
              export EM_PORTS=$PWD/test
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
