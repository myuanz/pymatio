curl -fsSL https://xmake.io/shget.text | bash &&
echo 'alias python=python3' >> ~/.xmake/profile &&
echo 'source ~/.xmake/profile' >> ~/.bashrc &&
export XMAKE_ROOT="y" &&
echo $PATH