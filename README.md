This game uses a server manager to work correctly.
Download the code from the link: https://github.com/EBobrikow/ServerManager.
Edit the Config.json file. Change the path to Server.exe. The name will be TowerOffenseServer.exe. You also need to change the IP address where the server manager is deployed.
Also in the TOGameInstance.cpp file in the "ConnectToServerManager()" function, change the URL to the address where your server manager is deployed.
