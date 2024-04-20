<div class="title-block" style="text-align: center;" align="center">

# LigmaOS

A 32bit operating system for i686 systems.

</div>

## Getting dependencies to build and develop

### The easy way

1. Install the [Nix package manager](https://nixos.org/download/).
2. Run this command: `nix --experimental-features "nix-command flakes" develop ./flake.nix`

This will give you a shell with all that is required to build and run this OS on a linux system.
It might take some time to build the cross compiler so you will have to be patient.

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

We use **gnu make** to automate the build of the OS and **qemu** to run it in a VM.

To build the OS you can run this command from the root of the repo `make`.

To build and run the OS you can use this command from the root of the repo `make run`.

To debug the OS run `make debug` from the root of the repo.
This should open the debugger and the OS in qemu. The debugger is setup to break
on the first line of `KernelMain(void)`. Once the bootloader is done you should
be able to step through the OS. If you reach the end execution or have to restart the OS
you can close the qemu window and reopen it again with `make debug`. To reattach the debugger
to this new VM window you can type `g` mnemonic for "go" in the debugger. Then you can step
through the OS again without loosing your debuggin session.
