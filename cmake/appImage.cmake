# appImage.cmake

if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

if(GENERATE_APPIMAGE)
    if(UNIX AND NOT APPLE)
        message(STATUS "GENERATE_APPIMAGE is ON")

        #downlaod linuxdeploy

         add_custom_target(download-linuxdeploy ALL
            COMMAND wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage 
            COMMAND chmod +x linuxdeploy-x86_64.AppImage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

        #set paths
        set(APPIMAGE_DIR "${CMAKE_BINARY_DIR}/AppDir")
        set(APP_BIN_DIR "${APPIMAGE_DIR}/usr/bin")

        file(MAKE_DIRECTORY ${APP_BIN_DIR})
        file(MAKE_DIRECTORY ${APPIMAGE_DIR}/usr/share/metainfo)
        file(MAKE_DIRECTORY ${APPIMAGE_DIR}/usr/lib)

        #move DeskUp executable to bin
        add_custom_command(TARGET DeskUp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:DeskUp> ${APP_BIN_DIR}/DeskUp
        )

        #create and write AppRun
        file(WRITE "${APPIMAGE_DIR}/AppRun" 
"#!/bin/bash
HERE=\"\$(dirname \"\$(readlink -f \"\$0\")\")\"

export LD_LIBRARY_PATH=\"$HERE/usr/lib:\$LD_LIBRARY_PATH\"
echo \$LD_LIBRARY_PATH

FLAG=\"\$HOME/.local/share/applications/.deskup_installed\"

if [ ! -f \"\$FLAG\" ]; then
    \"\$HERE/integrate.sh\"
    touch \"\$FLAG\"
fi

case \"\$1\" in
    --uninstall)
        echo \"Uninstalling DeskUp...\"
        \"\$HERE/uninstall.sh\"
        exit 0
        ;;
esac

exec \"\$HERE/usr/bin/DeskUp\" \"\$@\"
")

        execute_process(COMMAND chmod +x "${APPIMAGE_DIR}/AppRun")

        #create and write .desktop
        file(WRITE ${APPIMAGE_DIR}/DeskUp.desktop "[Desktop Entry]
Terminal=false
Type=Application
Name=DeskUp
Exec=AppRun
Icon=DeskUp-128x128
Categories=Utility
StartupWMClass=DeskUp;"
        )

          #create and write uninstall.sh
        file(WRITE ${APPIMAGE_DIR}/uninstall.sh "#!/bin/bash
TARGET_DIR=\"\$HOME/Applications\"
APP_NAME=\"DeskUp\"
LOCAL_SHARE=\"\$HOME/.local/share\"

rm -f \$TARGET_DIR/\$APP_NAME.AppImage

rm -f \$LOCAL_SHARE/icons/hicolor/32x32/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/64x64/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/48x48/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/96x96/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/128x128/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/256x256/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/512x512/apps/\$APP_NAME.png

rm -f \$LOCAL_SHARE/applications/$APP_NAME.desktop

rm -f \"\$LOCAL_SHARE/applications/.deskup_installed\"

update-desktop-database ~/.local/share/applications >/dev/null 2>&1 || true")

        execute_process(COMMAND chmod +x "${APPIMAGE_DIR}/uninstall.sh")

        #move all the icons
        file(COPY ${CMAKE_SOURCE_DIR}/assets/DeskUp-16x16.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/DeskUp-32x32.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/DeskUp-48x48.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/DeskUp-64x64.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/DeskUp-96x96.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/DeskUp-128x128.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/DeskUp-256x256.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/DeskUp-512x512.png DESTINATION ${APPIMAGE_DIR})

        #create and write integrate.sh

        file(WRITE "${APPIMAGE_DIR}/integrate.sh" "#!/bin/bash   

APPDIR=\"\$(dirname \"\$(readlink -f \"\$0\")\")\"

APP_NAME=\"DeskUp\"

TARGET_DIR=\"\$HOME/Applications\"

APPIMAGE_PATH=\"\${APPIMAGE:-\$\(readlink -f \"\$0\"\)}\"

LOCAL_SHARE=\"\$HOME/.local/share\"

mkdir -p \"\$TARGET_DIR\"

cp \"\$APPIMAGE_PATH\" \"\$TARGET_DIR/\$APP_NAME.AppImage\"
chmod +x \"\$TARGET_DIR/\$APP_NAME.AppImage\"

mkdir -p \$LOCAL_SHARE/applications
mkdir -p \$LOCAL_SHARE/icons
mkdir -p \$LOCAL_SHARE/icons/hicolor/32x32/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/64x64/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/48x48/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/96x96/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/128x128/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/256x256/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/512x512/apps

cat > \$LOCAL_SHARE/applications/$APP_NAME.desktop <<EOL
[Desktop Entry]
Name=\$APP_NAME
GenericName=Window Manager
GenericName[en]=Window Manager
GenericName[es]=Gestor de Ventanas
Exec=\$TARGET_DIR/\$APP_NAME.AppImage
Icon=\$APP_NAME
Terminal=false
Type=Application
Keywords=Desktop;Window;Organize;Manager;
Categories=Utility;
Comment[en]=Keep your workspace organized
Comment[es]=Organiza tu espacio de trabajo
StartupNotify=true
StartupWMClass=DeskUp

Actions=Uninstall;
[Desktop Action Uninstall]
Name=Uninstall
Name[en]=Uninstall
Name[es]=Desinstalar
Exec=\$TARGET_DIR/\$APP_NAME.AppImage --uninstall
EOL

chmod +x \$LOCAL_SHARE/applications/$APP_NAME.desktop


cp \"\$APPDIR/\$APP_NAME-32x32.png\" \$LOCAL_SHARE/icons/hicolor/32x32/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-64x64.png\" \$LOCAL_SHARE/icons/hicolor/64x64/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-48x48.png\" \$LOCAL_SHARE/icons/hicolor/48x48/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-96x96.png\" \$LOCAL_SHARE/icons/hicolor/96x96/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-128x128.png\" \$LOCAL_SHARE/icons/hicolor/128x128/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-256x256.png\" \$LOCAL_SHARE/icons/hicolor/256x256/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-512x512.png\" \$LOCAL_SHARE/icons/hicolor/512x512/apps/\$APP_NAME.png

update-desktop-database \$LOCAL_SHARE/applications >/dev/null 2>&1 || true

echo \"\$APP_NAME installed successfully!\"")

        execute_process(COMMAND chmod +x "${APPIMAGE_DIR}/integrate.sh")

        add_custom_target(appimage ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Generating appImage..."
            COMMAND ./linuxdeploy-x86_64.AppImage --appdir AppDir --executable AppDir/usr/bin/DeskUp --exclude-library=libc.so.* --exclude-library=libm.so.* --output appimage
            DEPENDS DeskUp
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

        add_custom_target(delete-linuxdeploy ALL
            COMMAND rm -f linuxdeploy-x86_64.AppImage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    else()
        message(STATUS "You cannot generate an AppImage on Windows!")
    endif()
else()
    message(STATUS "GENERATE_APPIMAGE is OFF")
endif()