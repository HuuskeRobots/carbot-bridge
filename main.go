package main

import (
	"flag"
	"log"
	"net"
	"strconv"
	"time"
)

const (
	defaultIP = "192.168.4.1"
)

var (
	robotIP    string // IP address of robot
	robotPort  int    // Port on robot
	robotName  string // Hostname of robot
	serverPort int    // Port on which to listen

	projectVersion = "dev"
	projectBuild   = "dev"
)

func init() {
	flag.StringVar(&robotIP, "ip", "", "IP address of the robot")
	flag.IntVar(&robotPort, "port", 80, "Port of the robot")
	flag.StringVar(&robotName, "name", "", "MDNS Name of the robot")
	flag.IntVar(&serverPort, "server-port", 26153, "Port on which our server will listen")
}

func main() {
	flag.Parse()

	log.Printf("Starting Auto-Bridge version %s, build %s\n", projectVersion, projectBuild)

	attempt := 0
	for {
		if attempt > 0 {
			log.Println("Waiting a bit...")
			time.Sleep(time.Second * 5)
		}
		attempt++
		ip := robotIP
		if robotName == "" && ip == "" {
			ip = defaultIP
		} else if ip == "" {
			var err error
			ip, err = findRobotIP(robotName)
			if err != nil {
				log.Printf("Kan robot '%s' niet vinden\n", robotName)
				continue
			}
		}

		// Now start the server
		robotAddr := net.JoinHostPort(ip, strconv.Itoa(robotPort))
		shutdownServer := make(chan struct{})
		s, err := NewServer(robotAddr, serverPort, shutdownServer)
		if err != nil {
			log.Printf("Kan server niet starten: %#v\n", err)
			continue
		}
		log.Printf("Listening on port %d\n", serverPort)
		go func() {
			if err := s.ListenAndServe(); err != nil {
				log.Fatalf("Kan de server niet starten: %#v\n", err)
			}
		}()
		<-shutdownServer
		log.Printf("Shutting down server...\n")
		if err := s.Shutdown(); err != nil {
			log.Printf("Failed to shutdown server: %#v\n", err)
		}
	}
}
