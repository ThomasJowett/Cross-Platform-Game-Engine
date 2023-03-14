CURRENT_DIR=$(basename "$PWD")

if [[ "$CURRENT_DIR" == "Scripts" ]]; then
    cd ..
fi
./vendor/bin/premake/premake5_arm gmake2 -os macosx -arch="arm64"
