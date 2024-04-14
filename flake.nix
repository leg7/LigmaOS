{
  description = "Environment to build and test an operating system";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

  outputs = { nixpkgs, ... }:
  let system = "x86_64-linux";
  in
  {
    devShells."${system}".default =
    let pkgs = import nixpkgs { inherit system; };
    in pkgs.mkShell
    {
      packages = with pkgs; [
        # boot
        grub2
        xorriso
        # build/run
        gnumake
        qemu
        nasm
        # debug
        halloy
        gdb
        gf
        fish
      ];

      # Cross compiler
      shellHook = ''
      nix-shell crossShell.nix
      '';
    };
  };
}
