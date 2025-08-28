{
  description = "a cpp devshell";

  outputs = { self, nixpkgs, flake-utils }:
  flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs {inherit system;};
    in 
    {
      devShell = with pkgs; mkShell {
        nativeBuildInputs = [ bear clang ];
        buildInputs = [ clang ];
        packages = with pkgs; [
          cmake
          boost
        ];
      };
    }
  );

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
}
