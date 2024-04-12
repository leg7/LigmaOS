let
  unstableTarball = fetchTarball https://github.com/NixOS/nixpkgs/archive/nixos-unstable.tar.gz;
in with import unstableTarball {
  crossSystem = {
    config = "i686-elf";
  };
};

mkShell {
  buildInputs = [];
  shellHook = "exec fish";
}

