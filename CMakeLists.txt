cmake_minimum_required(VERSION 3.16)

project(ExpenseTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Sql)

qt_standard_project_setup()

qt_add_executable(ExpenseTracker
    src/main.cpp
    src/AppController.cpp
    src/AppController.h
    src/db/Database.cpp
    src/db/Database.h
    src/models/TransactionsModel.cpp
    src/models/TransactionsModel.h
)

qt_add_qml_module(ExpenseTracker
    URI ExpenseTracker
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/DashboardCard.qml
        qml/components/FiltersBar.qml
        qml/components/TransactionDialog.qml
        qml/components/TransactionDelegate.qml
        qml/components/EmptyState.qml
        qml/components/Snackbar.qml
)

target_link_libraries(ExpenseTracker PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Sql)

install(TARGETS ExpenseTracker
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
