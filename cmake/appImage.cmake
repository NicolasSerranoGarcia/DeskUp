# appImage.cmake

if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

if(GENERATE_APPIMAGE)
    if(UNIX AND NOT APPLE)
        message(STATUS "GENERATE_APPIMAGE is ON")

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
    echo integrate
    touch \"\$FLAG\"
fi

exec \"\$HERE/usr/bin/DeskUp\" \"\$@\"
")

        execute_process(COMMAND chmod +x "${APPIMAGE_DIR}/AppRun")

        #create and write .desktop
        file(WRITE ${APPIMAGE_DIR}/DeskUp.desktop "[Desktop Entry]
Terminal=false
Type=Application
Name=DeskUp
Exec=AppRun
Icon=DeskUp
Categories=Utility
StartupWMClass=DeskUp;"
        )

        #move the icon
        file(COPY ${CMAKE_SOURCE_DIR}/assets/DeskUp.png DESTINATION ${APPIMAGE_DIR})

        #create and write integrate.sh

        file(WRITE "${APPIMAGE_DIR}/integrate.sh" "#!/bin/bash   

APPDIR=\"\$(dirname \"\$(readlink -f \"\$0\")\")\"

APP_NAME=\"DeskUp\"

TARGET_DIR=\"\$HOME/Applications\"

APPIMAGE_PATH=\"\${APPIMAGE:-\$\(readlink -f \"\$0\"\)}\"

mkdir -p \"\$TARGET_DIR\"

cp \"\$APPIMAGE_PATH\" \"\$TARGET_DIR/\$APP_NAME.AppImage\"
chmod +x \"\$TARGET_DIR/\$APP_NAME.AppImage\"

mkdir -p ~/.local/share/applications
mkdir -p ~/.local/share/icons

cat > ~/.local/share/applications/$APP_NAME.desktop <<EOL
[Desktop Entry]
Name=\$APP_NAME
Comment=Keep your workspace organized
Exec=\$TARGET_DIR/\$APP_NAME.AppImage
Icon=DeskUp
Terminal=false
Type=Application
Categories=Utility;
EOL

cp \"\$APPDIR/DeskUp.png\" ~/.local/share/icons/hicolor/32x32/apps/\$APP_NAME.png
cp \"\$APPDIR/DeskUp.png\" ~/.local/share/icons/hicolor/64x64/apps/\$APP_NAME.png
cp \"\$APPDIR/DeskUp.png\" ~/.local/share/icons/hicolor/48x48/apps/\$APP_NAME.png
cp \"\$APPDIR/DeskUp.png\" ~/.local/share/icons/hicolor/96x96/apps/\$APP_NAME.png
cp \"\$APPDIR/DeskUp.png\" ~/.local/share/icons/hicolor/128x128/apps/\$APP_NAME.png
cp \"\$APPDIR/DeskUp.png\" ~/.local/share/icons/hicolor/256x256/apps/\$APP_NAME.png
cp \"\$APPDIR/DeskUp.png\" ~/.local/share/icons/hicolor/512x512/apps/\$APP_NAME.png

update-desktop-database ~/.local/share/applications >/dev/null 2>&1 || true

echo \"DeskUp installed successfully!\"")

        execute_process(COMMAND chmod +x "${APPIMAGE_DIR}/integrate.sh")

        add_custom_target(appimage ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Generating appImage..."
            COMMAND appimagetool ${APPIMAGE_DIR} ./DeskUp-x86_64.AppImage
            DEPENDS DeskUp
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    else()
        message(STATUS "You cannot generate an AppImage on Windows!")
    endif()
else()
    message(STATUS "GENERATE_APPIMAGE is OFF")
endif()