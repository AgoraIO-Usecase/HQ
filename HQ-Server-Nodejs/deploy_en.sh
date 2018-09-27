#!/bin/sh
#copy client static files

deployDir=/home/test-pc/HQ-PHASE-3
ServerName=${@:$OPTIND:1}

# Rev="$(git rev-parse HEAD)"

# echo $Rev
# cd dist
cp modules/config_en.js modules/config.js
rsync -v -z -r --progress -h --exclude=node_modules/* --exclude=.* --exclude=deploy_test.sh . $ServerName:$deployDir

echo service deployed on:$ServerName:$deployDir
