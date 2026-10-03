{
  description = "FurryOS freestanding x86 development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    in {
      devShells = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
        in {
          default = pkgs.mkShell {
            packages = with pkgs; [
              clang
              clang-tools
              lld
              llvm
              nasm
              gnumake
              qemu
              gdb
              binutils
              mtools
              dosfstools
            ];

            CC = "clang";
            LD = "ld.lld";
            AS = "nasm";

            shellHook = ''
              echo "FurryOS dev shell: clang $(clang --version | head -n 1)"
              echo "Targets: BIOS loader + freestanding i686 kernel"
            '';
          };
        });
    };
}
