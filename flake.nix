{
  description = "A Nix-flake-based C/C++ development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let
    supportedSystems = [ "x86_64-linux" "aarch64-linux" ];
    forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
      pkgs = import nixpkgs { 
          inherit system; 
      };
    });
  in {
    devShells = forEachSupportedSystem ({ pkgs }: {
      default = pkgs.mkShell.override {}
        {
          shellHook = /* bash */ ''
            export LD_LIBRARY_PATH=${pkgs.glew}/lib:$LD_LIBRARY_PATH
          '';

          packages = with pkgs; [
              ninja
              cmake
              extra-cmake-modules
              cmakeCurses 
              clang-tools
          ];

          buildInputs = with pkgs; [
            gcc
            libffi
          ];

          nativeBuildInputs = with pkgs; [
            SDL2
            glew 
            glm 
            libGL
            # wayland-protocols
            # wayland
            # libxkbcommon
          ];
        };
    });
  };
}
