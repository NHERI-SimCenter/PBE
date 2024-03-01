

#
# parse args
#

METHOD="new"

while [[ "${1:0:1}" = "-" ]]; do
    case $1 in
	--old)
	    METHOD="old"
	    shift;;
	--new)
	    METHOD="new"
	    shift;;	
    esac
done

#
# Paramaters
#

APP_NAME="PBE"
APP_FILE="PBE.app"
DMG_FILENAME="PBE_Mac_Download.dmg"

QTDIR="/Users/fmckenna/Qt/5.15.2/clang_64/"

pathToBackendApps="/Users/fmckenna/NHERI/SimCenterBackendApplications"
pathToOpenSees="/Users/fmckenna/bin/OpenSees3.6.0"
pathToDakota="/Users/fmckenna/dakota-6.12.0"
pathToQuaZip="/Users/fmckenna/NHERI/quazip/build/quazip"


#
# build it
# 

./makeEXE.sh
cd build

#
# Check to see if the app built
#

if ! [ -x "$(command -v open $pathApp)" ]; then
	echo "$APP_FILE did not build. Exiting."
	exit 
fi

cp -fr $HOME/NHERI/s3hark/resources ./PBE.app/Contents/MacOS

#
# macdeployqt it
#

macdeployqt ./PBE.app -qmldir=$HOME/NHERI/s3hark

#
# add missing files from macdeployqt (a known bug)
#


#mkdir -p ./$APP_FILE/Contents/plugins/renderers/
#cp -R $QTDIR/plugins/renderers/libopenglrenderer.dylib ./$APP_FILE/Contents/plugins/renderers/
#mkdir -p ./$APP_FILE/Contents/plugins/renderplugins/
#cp -R $QTDIR/plugins/renderplugins/libscene2d.dylib ./$APP_FILE/Contents/plugins/renderplugins/

#
# copy backend
#  1. copy all
#  2. remove unwanted

cp -fR $pathToBackendApps/applications ./$APP_FILE/Contents/MacOS

# remove unwanted
declare -a notWantedApp=("createBIM" 
			 "performRegionalMapping"
			 "performRegionalEventSimulation"
			 "createEDP/standardEarthquakeEDP_R"
			 "createEDP/userEDP_R"
			 "createEDP/simpleEDP"
			 "createEDP/standardWindEDP"
			 "createEVENT/ASCE7_WindSpeed"
			 "createEVENT/CFDEvent"
			 "createEVENT/hazardBasedEvent"
			 "createEVENT/DEDM_HRP"
			 "createEVENT/stochasticWind"
			 "createEVENT/stochasticWind"
			 "createEVENT/ASCE7_WindSpeed"
			 "createEVENT/LLNL_SW4"
			 "createEVENT/windTunnelExperiment"
			 "createEVENT/LowRiseTPU"
			 "createEVENT/pointWindSpeed"
			 "createSAM/MDOF-LU"
			 "performSIMULATION/IMasEDP"
			 "performSIMULATION/extractPGA"
			 "performSIMULATION/openSees_R"
			)

for app in "${notWantedApp[@]}"
do
   echo "removing $app"
   rm -fr ./$APP_FILE/Contents/MacOS/applications/$app
done

#
# copy applications folder opensees, dakota and quazip
#

cp $pathToQuaZip/libquazip1-qt5.1.4.0.dylib ./$APP_FILE/Contents/Frameworks/
cp $pathToQuaZip/libquazip1-qt5.1.4.dylib ./$APP_FILE/Contents/Frameworks/
cp $pathToQuaZip/libquazip1-qt5.dylib ./$APP_FILE/Contents/Frameworks/
mkdir  ./$APP_FILE/Contents/MacOS/applications/opensees
mkdir  ./$APP_FILE/Contents/MacOS/applications/dakota
cp -fr $pathToOpenSees/* ./$APP_FILE/Contents/MacOS/applications/opensees
cp -fr $pathToDakota/*  ./$APP_FILE/Contents/MacOS/applications/dakota

#
# clean up
#

find ./$APP_FILE -name __pycache__ -exec rm -rf {} +;


#
# load my credential file
#

userID="../userID.sh"

if [ ! -f "$userID" ]; then

    echo "creating dmg $DMG_FILENAME"
    hdiutil create $DMG_FILENAME -fs HFS+ -srcfolder ./$APP_FILE -format UDZO -volname $APP_NAME

    echo "No password & credential file to continue with codesig and App store verification"
    exit
fi

source $userID
echo $appleID


#
# create dmg
#

echo "$METHOD"

if test "$METHOD" = "new"
then
    
    #
    # mv app into empty folder for create-dmg to work
    # brew install create-dmg
    #

    echo "codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $APP_FILE"
    codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $APP_FILE    
    
    mkdir app
    mv $APP_FILE app
    
    # swoop
    #create-dmg \
	#  --volname "${APP_NAME}" \
	#  --background "../background/background1.png" \
	#  --window-pos 200 120 \
	#  --window-size 550 400 \
	#  --icon-size 150 \
	#  --icon "${APP_NAME}.app" 150 190 \
	#  --hide-extension "${APP_NAME}.app" \
	#  --app-drop-link 400 185 \
	#  "${DMG_FILENAME}" \
	#  "app"
    
    # vertical 
    #create-dmg \
	#  --volname "${APP_NAME}" \
	#  --background "../background/background2.png" \
	#  --window-pos 200 120 \
	#  --window-size 475 550 \
	#  --icon-size 150 \
	#  --icon "${APP_NAME}.app" 235 125 \
	#  --hide-extension "${APP_NAME}.app" \
	#  --app-drop-link 235 400 \
	#  "${DMG_FILENAME}" \
	#  "app"
    
    #horizontal
    ../macInstall/create-dmg \
	--volname "${APP_NAME}" \
	--background "../macInstall/background3.png" \
	--window-pos 200 120 \
	--window-size 600 350 \
	--no-internet-enable \
	--icon-size 125 \
	--icon "${APP_NAME}.app" 125 130 \
	--hide-extension "${APP_NAME}.app" \
	--app-drop-link 450 130 \
	--codesign $appleCredential \
	"${DMG_FILENAME}" \
	"app"
    
    #  --notarize $appleID $appleAppPassword \
	
    mv ./app/$APP_FILE ./
    rm -fr app

else

    echo "codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $APP_FILE"
    codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $APP_FILE
        
    echo "hdiutil create $DMG_FILENAME -fs HFS+ -srcfolder ./$APP_FILE -format UDZO -volname $APP_NAME"
    hdiutil create $DMG_FILENAME -fs HFS+ -srcfolder ./$APP_FILE -format UDZO -volname $APP_NAME

    echo "Issue: codesign --force --sign "$appleCredential" $DMG_FILENAME"
    #codesign --force --sign "$appleCredential" $DMG_FILENAME
    
fi

echo "xcrun altool --notarize-app -u $appleID -p $appleAppPassword -f ./$DMG_FILENAME --primary-bundle-id altool --verbose"
echo ""
echo "returns id: ID .. wait for email indicating success"
echo "To check status"
echo "xcrun altool --notarization-info ID  -u $appleID  -p $appleAppPassword"
echo ""
echo "Finally staple the dmg"
echo "xcrun stapler staple \"$APP_NAME\" $DMG_FILENAME"
