echo "Install Linux Dependencies"

echo "apt"
apt update

apt install libegl1 -y
apt install libglx-dev -y
apt install qt6-base-dev -y

echo "apt-get"
apt-get update 
apt-get install -y libgl1-mesa-dev
apt-get install -y libxkbcommon-dev
