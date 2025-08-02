#!/usr/bin/env sh

GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
RED=$(tput setaf 1)
CYAN=$(tput setaf 6)
BRIGHT=$(tput bold)
RESET=$(tput sgr0)

PROJECT=${PWD##*/}
LIB="ctk"
LIB_URL="https://github.com/nate-craft/${LIB}.git"
RELEASE=false
CACHED=false
SYSTEM_INSTALL=false
RUN_TYPE=""

HELP="
${CYAN}${BRIGHT}$(basename "$PWD")${RESET} - built with cproject

${CYAN}${BRIGHT}Flags:${RESET}
  ${GREEN}[ ${ITALIC}--clean${RESET}   ${GREEN}| ${GREEN}${ITALIC}-C${RESET}${GREEN} ]:   ${YELLOW}(removes build directories)
  ${GREEN}[ ${ITALIC}--cached${RESET}  ${GREEN}| ${GREEN}${ITALIC}-c${RESET}${GREEN} ]:   ${YELLOW}(builds without downloading the ${LIB} library)
  ${GREEN}[ ${ITALIC}--run${RESET}     ${GREEN}| ${GREEN}${ITALIC}-r${RESET}${GREEN} ]:   ${YELLOW}(runs the built file)
  ${GREEN}[ ${ITALIC}--debug${RESET}   ${GREEN}| ${GREEN}${ITALIC}-d${RESET}${GREEN} ]:   ${YELLOW}(runs the built file with valgrind)
  ${GREEN}[ ${ITALIC}--release${RESET} ${GREEN}| ${GREEN}${ITALIC}-R${RESET}${GREEN} ]:   ${YELLOW}(builds with O3 compiler flags)
  ${GREEN}[ ${ITALIC}--system${RESET}  ${GREEN}| ${GREEN}${ITALIC}-s${RESET}${GREEN} ]:   ${YELLOW}(installs executable system-wide)
  ${GREEN}[ ${ITALIC}--delete${RESET}  ${GREEN}| ${GREEN}${ITALIC}-D${RESET}${GREEN} ]:   ${YELLOW}(uninstalls executable system-wide)

${CYAN}${BRIGHT}Examples:${RESET}
  ${GREEN}./build.sh --clean                   ${YELLOW}(clean build directories)
  ${GREEN}./build.sh --cached                  ${YELLOW}(build without library install)
  ${GREEN}./build.sh --release                 ${YELLOW}(build without library install with O3 option)
  ${GREEN}./build.sh --cached --run            ${YELLOW}(build and run without library install)
  ${GREEN}./build.sh --cached --run --release  ${YELLOW}(build and run without library install with O3 option)
  ${GREEN}./build.sh --cached --debug          ${YELLOW}(build and debug without library install)
  ${GREEN}./build.sh --system                  ${YELLOW}(build and install system-wide)
  ${GREEN}./build.sh --delete                  ${YELLOW}(uninstall executable system-wide)
  ${GREEN}./build.sh                           ${YELLOW}(build without running)

"

msg() {
    printf "%s%s%s\n" "$GREEN" "$@" "$RESET"  
}

panic() {	
    printf "%s%s%s%s\n" "$RED" "[BUILD ERROR]: " "$@" "$RESET"  
	exit 1
}

help() {
	printf "%s%s\n" "$HELP" "$RESET"  
	exit 1
}

build() {
    rm -rf build
    rm -rf out
    mkdir build
    mkdir out
	(
		cd build || panic "Could not move into build directory" 

		if [ "$RELEASE" = "true" ]; then
			cmake -DCMAKE_BUILD_TYPE=Release ..
		else
			cmake -DCMAKE_BUILD_TYPE=Debug ..
		fi

		if [ "$SYSTEM_INSTALL" = "true" ]; then
			sudo make install
		else
			make
			cp "$PROJECT" ../out/
		fi
	)    
}

libs() {
	git clone "$LIB_URL"
	(
		cd "$LIB" || panic "Could not move into ${LIB} directory" 
		chmod +x build.sh
		./build.sh --local
		cp -r out/* ../
	)
	rm -rf "$LIB"
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        -h|--help)
			help
            ;;
        -C|--clean)
            rm -rf build
            rm -rf out
            rm -rf include
            rm -rf lib
            msg 'Directories have been cleared!'
            exit 0
            ;;
        -c|--cached)
            CACHED=true
            shift
            ;;
        -d|--debug)
            RUN_TYPE="debug"
            RELEASE=false
            shift
            ;;
        -r|--run)
            RUN_TYPE="run"
            shift
            ;;
        -R|--release)
            RELEASE=true
            shift
            ;;
        -S|--system)
            SYSTEM_INSTALL=true
            shift
            ;;
        -u|--uninstall)
            sudo rm -I /usr/local/bin/"$PROJECT"
            exit 0
            ;;
        *)
            break
            ;;
    esac
done

if ! "$CACHED" || [ ! -d lib ]; then
    libs
fi

build

if [ "$RUN_TYPE" = "run" ]; then
    "${PWD}/out/${PROJECT}" "$@"
elif [ "$RUN_TYPE" = "debug" ]; then
    valgrind -s --leak-check=full --track-origins=yes "${PWD}/out/${PROJECT}" "$@"
fi

