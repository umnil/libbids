echo "Install Linux Dependencies"

echo "apt"
apt update

apt install libegl1 -y
apt install libglx-dev -y
apt install qt6-base-dev qt6-base-private-dev qt6-declarative-dev qt6-declarative-private-dev qt6-tools-dev qt6-tools-private-dev qt6-scxml-dev qt6-documentation-tools libqt6core5compat6-dev qt6-tools-dev-tools qt6-l10n-tools qt6-shader-baker libqt6shadertools6-dev qt6-quick3d-dev qt6-quick3d-dev-tools libqt6svg6-dev libqt6quicktimeline6-dev libqt6serialport6-dev -y
apt install libgl1-mesa-dev libvulkan-dev libxcb-xinput-dev libxcb-xinerama0-dev libxkbcommon-dev libxkbcommon-x11-dev libxcb-image0 libxcb-keysyms1 libxcb-render-util0 libxcb-xkb1 libxcb-randr0 libxcb-icccm4

#echo "apt-get"
#add-apt-repository ppa:oibaf/graphics-drivers
#apt-get update 
#apt-get upgrade
#apt-get install -y libgl1-mesa-dev
#apt-get install -y libxkbcommon-dev
#apt-get install -y libvulkan1 mesa-vulkan-drivers vulkan-tools
