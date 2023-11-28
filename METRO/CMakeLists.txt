# CMakeList.txt : projet CMake pour VAL Modélisation (LOCAL), incluez la source et définissez
# la logique spécifique au projet ici.
#
Project ("VAL Modélisation (LOCAL)")
# Ajoutez une source à l'exécutable de ce projet.
add_executable (CMakeTarget "METRO.cpp" "METRO.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.


