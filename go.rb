require 'zmq'

module Go
  CONTEXT = ZMQ::Context.new(1)

  class << self
    attr_accessor :template_server

    def render(request)
      self.template_server ||=  "tcp://localhost:9000"
      @server ||= setup
      send(request)
    end

    # FIXME this is being reloaded in devlopment mode
    # Should only be called once per process
    # ZMQ only wants 1 context per process
    def setup
      @timeout = 2
      client_sock
      at_exit do
        @socket.close
      end
    end

    def client_sock
      @socket.close if @socket
      @socket = CONTEXT.socket(ZMQ::REQ)
      @socket.setsockopt(ZMQ::LINGER, 0)
      @socket.connect(self.template_server)
    end

    def send(message)
      raise("Send: #{message} failed") unless @socket.send(message)

      if ZMQ.select( [@socket], nil, nil, @timeout)
        @socket.recv
      else
        client_sock
        "Template server is down"
      end
    end
  end
end
