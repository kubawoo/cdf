#!/bin/bash -e

export CDF_HOME=${CDF_HOME:-"${HOME}/.cdf"}
CURR_DIR=`pwd`

function need_to_build() {
    group=$1
    name=$2
    ver=`echo $3 | sed 's|-|.|' | awk -F"." '{if ($4) {print $1"."$2"-"$4} else {print $1"."$2}}'`

    echo "Checking if $group:$name:$ver exists..."
    path="${CDF_HOME}/${group}/${name}/${ver}"

    if [ -d "$path" ]; then
        echo "Found in $path"
        return 1
    fi
    return 0
}


modules=`jq -r '.dependencies | .[] | select(.group=="cdf") | .name' "$CURR_DIR/cdfmodule.json"`

echo "Will run in $CDF_HOME"
mkdir -p $CDF_HOME/build
cd $CDF_HOME/build

for m in $modules; do
    version=`jq -r ".dependencies | .[] | select(.group==\"cdf\") | select(.name==\"$m\") | .version" $CURR_DIR/cdfmodule.json`
    if need_to_build "cdf" "$m" "$version"; then
        echo "Building $m v$version"
        cd $CURR_DIR/../$m
        make
        make test
        make install
        cd $CURR_DIR
    else
        echo "Dependency $m v$version already exists"
    fi
done;

cd $CURR_DIR
rm -rf $CDF_HOME/build

make
make install
