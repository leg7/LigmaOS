<div class="title-block" style="text-align: center;" align="center">

# LigmaOS

A 32bit operating system for i686 systems.

</div>

## Getting dependencies to build and develop

### The easy way

1. Install the [Nix package manager](https://nixos.org/download/).
2. Run this command: `nix develop ./flake.nix`

This will give you a shell with all that is required to build and run this OS on a linux system.
It takes some time to build the cross compiler so you will have to be patient.

### The hard way

Install these packages such that the versions match those of the
[nixpkgs repo](https://github.com/NixOS/nixpkgs) at the commit in `flake.lock`.

- grub2
- xorriso
- gnumake
- nasm
- gdb
- gf
- qemu
- fish

Once this is done you will have to build a cross compiler for i686.
[Here's a guide](https://osdev.org/GCC_Cross-Compiler) on how to do it.
If you don't have a cross compiler the operating system will not work.

## Building and running

We use gnumake to automate the build of the OS and qemu to run it in a VM.
To run the OS use this command from the src directory

`make run`
