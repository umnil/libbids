echo "Install Linux Dependencies"

echo "apt"
apt update

apt install libegl1 -y
apt install libglx-dev -y
apt install qt6-base-dev -y

echo "apt-get"
add-apt-repository ppa:oibaf/graphics-drivers
apt-get update 
apt-get upgrade
apt-get install -y libgl1-mesa-dev
apt-get install -y libxkbcommon-dev
apt-get install -y libvulkan1 mesa-vulkan-drivers vulkan-utils
