package main

import (
	"errors"
	"log"
	"strings"
	"time"

	"github.com/oleksandr/bonjour"
)

func findRobotIP(robotName string) (string, error) {
	log.Printf("Zoeken naar '%s'...\n", robotName)
	resolver, err := bonjour.NewResolver(nil)
	if err != nil {
		return "", err
	}

	results := make(chan *bonjour.ServiceEntry)
	robotEntry := make(chan *bonjour.ServiceEntry)
	go func(results chan *bonjour.ServiceEntry, exitCh chan<- bool) {
		for e := range results {
			if strings.HasPrefix(e.Instance, robotName+".") {
				log.Printf("Robot gevonden op: '%s'\n", e.AddrIPv4.String())
				robotEntry <- e
				exitCh <- true
			}
			log.Printf("gevonden: '%s', '%s' %s:%d\n", e.Instance, e.Service, e.AddrIPv4.String(), e.Port)
		}
	}(results, resolver.Exit)

	if err := resolver.Browse("", "local.", results); err != nil {
		return "", err
	}

	select {
	case e := <-robotEntry:
		ip := e.AddrIPv4.String()
		return ip, nil
	case <-time.After(time.Second * 15):
		return "", errors.New("Timeout")
	}
}
