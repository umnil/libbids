cpplint --filter=-build/c++11,-legal/copyright $(find include -name "*.hpp") $(find include -name "*.h") $(find src -name "*.cpp") $(find tests -name "*.*pp")
