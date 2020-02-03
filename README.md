# BasicChatClient
Simple local chat client for toying purpose

# Building instruction
  - mkdir bin
  - cd bin/
  - cmake ..
  - scan-build cmake --build . --config Release -- -j2

Should work on any major OS

# Usage

## Tracker
Optional.
Allow clients to retrieve easily current active client.
First launch will trigger the creation of a configuration file.

Configuration can be modified to adjust behavior:
`
{
	"port" : 8752,
	"thread_number" : 4
}
`

## Client
Command line client
Allow you to send and receive message from other client, and can pull client list from server.
First launche will trigger the creator of a configuration file, that you can modify:

`
{
	"listening_port" : 5750,
	"server_ip" : "localhost",
	"server_port" : "8752",
	"thread_number" : 4,
	"username" : "tutu"
}
`

Current command line available :
-	`<user>|<message>`
-	`users` will get the list of currently logged users

example: `toto|hello world !`
example: `localhost:1337|comment est votre blanquette ?`

# Current Limitation | TODOs
-	Support of nat traversing to be tested, to be supported properly in next iteration
-	Current tests are lacking.
-	Simulatenous communication with a loot a client will lead to chaotic UI, a proper GUI should be done.
-	User authentification is non-existent, users can impersonate each others.
-	No ssl support yet, messages are not encrypted
-	Current memory pattern include allocation/deallocation in the main loop, which may lead to fragmented memory in the long run : moving to circular buffers & memory pools should greatly improve overall performances.

