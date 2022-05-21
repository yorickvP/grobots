{
  outputs = inputs@{ self, nixpkgs }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in {
    
    devShells.x86_64-linux.default = pkgs.mkShell {
      buildInputs = [ pkgs.meson pkgs.ninja pkgs.SDL2 pkgs.SDL2_ttf pkgs.SDL2_gfx ];
      nativeBuildInputs = [ pkgs.pkg-config ];
    };
  };

}
