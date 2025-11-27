# Unlike other cmake modules, this one just defines the qt interface library

    # --- fetch qt ---

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    # --- Create an interface library representing qt. Any sub-library that uses qt should link against this one ---

    add_library(desk_up_qt_library INTERFACE)
    target_link_libraries(desk_up_qt_library INTERFACE Qt6::Core Qt6::Widgets)

    # --- Acknowledge ---

    get_target_property(QT_CORE_INCLUDE_DIRS Qt6::Core INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(QT_WIDGETS_INCLUDE_DIRS Qt6::Widgets INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(QT_CORE_LIBS Qt6::Core LOCATION)
    get_target_property(QT_WIDGETS_LIBS Qt6::Widgets LOCATION)

    message(STATUS "(DU) Qt6 Core include dirs: ${QT_CORE_INCLUDE_DIRS}")
    message(STATUS "(DU) Qt6 Widgets include dirs: ${QT_WIDGETS_INCLUDE_DIRS}")
    message(STATUS "(DU) Qt6 Core lib: ${QT_CORE_LIBS}")
    message(STATUS "(DU) Qt6 Widgets lib: ${QT_WIDGETS_LIBS}")