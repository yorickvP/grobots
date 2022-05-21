{
  outputs = inputs@{ self, nixpkgs }: {
    packages.x86_64-linux.default = with nixpkgs.legacyPackages.x86_64-linux;
      stdenv.mkDerivation {
        pname = "grobots";
        version = "20220521";
        src = ./.;
        nativeBuildInputs = [ meson ninja pkg-config ];
        buildInputs = [ SDL2 SDL2_ttf SDL2_gfx ];
      };

    devShells.x86_64-linux.default = self.packages.x86_64-linux.default;
  };
}
