{
  description = "FileChain - blockchain based file versioning CLI";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          gcc
          gnumake
          openssl
          pkg-config
        ];

        shellHook = ''
          echo "FileChain dev shell"
          echo "Commands:"
          echo "  make      - compile"
          echo "  make run  - compile and run"
          echo "  make clean - delete compiled bin"
        '';
      };
    };
}
