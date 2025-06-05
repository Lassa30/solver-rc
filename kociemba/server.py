from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse, parse_qs
import argparse

from face import FaceCube
import cubie
from solver import solve


class RubikServer(BaseHTTPRequestHandler):
    # Global cube state
    cube = cubie.CubieCube()

    # Initialize basic move cubes
    basicMoveCube = cubie.basicMoveCube.copy()

    def do_GET(self):
        parsed = urlparse(self.path)
        path = parsed.path
        query = parse_qs(parsed.query)

        if path == "/move":
            self.handle_move(query)
        elif path == "/state":
            self.handle_state()
        elif path == "/solve":
            self.handle_solve()
        else:
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b"Not Found")

    def handle_move(self, query):
        if "move" not in query:
            self.send_response(400)
            self.end_headers()
            self.wfile.write(b"Missing move parameter")
            return

        move = query["move"][0].lower()

        # Map move to face index
        move_map = {"u": 0, "r": 1, "f": 2, "d": 3, "l": 4, "b": 5}

        if move not in move_map:
            self.send_response(400)
            self.end_headers()
            self.wfile.write(b"Invalid move")
            return

        # Apply the move
        self.cube.multiply(self.basicMoveCube[move_map[move]])

        # Return current state
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        self.wfile.write(self.cube.to_facelet_cube().to_string().encode())

    def handle_state(self):
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        self.wfile.write(self.cube.to_facelet_cube().to_string().encode())

    def handle_solve(self):
        # Convert to facelet cube
        fc = self.cube.to_facelet_cube()
        solution = solve(self.cube.to_facelet_cube().to_string(), 25, 1)

        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        self.wfile.write(solution.encode())


def run(server_class=HTTPServer, handler_class=RubikServer, port=8080):
    server_address = ("", port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting Rubik's Cube HTTP server on port {port}...")
    httpd.serve_forever()


if __name__ == "__main__":
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Rubik's Cube HTTP Server")
    parser.add_argument(
        "--port",
        type=int,
        default=8080,
        help="Port to run the server on (default: 8080)",
    )
    args = parser.parse_args()

    # Start server with the specified port
    run(port=args.port)
