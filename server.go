package main

import (
	"context"
	"fmt"
	"log"
	"net/http"
	"net/http/httputil"
	"net/url"
)

type Server struct {
	srv            http.Server
	shutdownServer chan struct{}
}

// NewServer prepares a new server
func NewServer(robotAddr string, port int, shutdownServer chan struct{}) (*Server, error) {
	s := &Server{
		shutdownServer: shutdownServer,
		srv: http.Server{
			Addr: fmt.Sprintf("0.0.0.0:%d", port),
		},
	}
	robotURL, err := url.Parse(fmt.Sprintf("http://%s", robotAddr))
	if err != nil {
		return nil, maskAny(err)
	}
	s.srv.Handler = s.createHandler(robotURL)
	return s, nil
}

// ListenAndServe starts listening on the desired port.
func (s *Server) ListenAndServe() error {
	err := s.srv.ListenAndServe()
	if err == http.ErrServerClosed {
		return nil
	}
	return maskAny(err)
}

// Shutdown gracefully stops the server
func (s *Server) Shutdown() error {
	return maskAny(s.srv.Shutdown(context.Background()))
}

func (s *Server) createHandler(robotURL *url.URL) *http.ServeMux {
	mux := http.NewServeMux()

	proxy := httputil.NewSingleHostReverseProxy(robotURL)
	mux.HandleFunc("/crossdomain.xml", s.crossDomainHandler)
	mux.Handle("/", proxy)

	return mux
}

const policy = `<cross-domain-policy>
  <allow-access-from-domain="*" to-port="26153"/>
</cross-domain-policy>\0`

func (s *Server) crossDomainHandler(w http.ResponseWriter, r *http.Request) {
	log.Println("crossDomain")
	w.Write([]byte(policy))
}
