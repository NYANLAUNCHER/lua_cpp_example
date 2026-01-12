{
  description = "Lua integration example using cpp.";

  inputs.nixpkgs.url = "nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };

    ##########################################
    pname = "lua_cpp_example";
    version = "0.1.0";
    nativeBuildInputs = with pkgs; [ # build dependencies
      gcc autoconf automake pkg-config rsync
    ];
    buildInputs = with pkgs; [ # runtime dependencies
      lua54Packages.lua
    ];
    ##########################################
  in {
    defaultPackage.${system} = pkgs.stdenv.mkDerivation {
      inherit pname version buildInputs nativeBuildInputs;
      src = ./.;
      buildPhase = ''
        make
      '';
      installPhase = ''
        make install
      '';
    };
    devShells.${system}.default = pkgs.mkShell {
      inherit buildInputs;
      nativeBuildInputs = with pkgs; [
        gdb unixtools.xxd
      ] ++ nativeBuildInputs;
      shellHook = ''
        export pname=${pname}
        make .ccls &> /dev/null
      '';
    };
  };
}
