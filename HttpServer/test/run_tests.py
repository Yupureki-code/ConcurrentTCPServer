#!/usr/bin/env python3

from __future__ import annotations

import hashlib
import os
import signal
import socket
import subprocess
import sys
import threading
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parent
SERVER_BIN = ROOT / "test"
WWWROOT = ROOT.parent / "src" / "wwwroot"
HOST = "127.0.0.1"
PORT = 18080
IDLE_TIMEOUT = 10
BUSINESS_TIMEOUT = 3


def ensure_large_file() -> Path:
    large_file = WWWROOT / "large.bin"
    pattern = bytes(range(256))
    target_size = 2 * 1024 * 1024
    with large_file.open("wb") as handle:
        remaining = target_size
        while remaining > 0:
            chunk = pattern[: min(len(pattern), remaining)]
            handle.write(chunk)
            remaining -= len(chunk)
    return large_file


def wait_for_port(host: str, port: int, timeout: float = 10.0) -> None:
    deadline = time.time() + timeout
    last_error: Exception | None = None
    while time.time() < deadline:
        try:
            with socket.create_connection((host, port), timeout=0.5):
                return
        except Exception as exc:  # noqa: BLE001
            last_error = exc
            time.sleep(0.1)
    raise RuntimeError(f"server did not start within {timeout}s: {last_error}")


def parse_headers(raw: bytes) -> tuple[int, dict[str, str], bytes]:
    header_end = raw.find(b"\r\n\r\n")
    if header_end < 0:
        raise RuntimeError("response missing header terminator")
    header_block = raw[:header_end].decode("latin1")
    body = raw[header_end + 4 :]
    lines = header_block.split("\r\n")
    status_parts = lines[0].split(" ", 2)
    if len(status_parts) < 2:
        raise RuntimeError(f"bad status line: {lines[0]!r}")
    status = int(status_parts[1])
    headers: dict[str, str] = {}
    for line in lines[1:]:
        if ": " in line:
            key, value = line.split(": ", 1)
            headers[key.lower()] = value
    return status, headers, body


def read_response(sock: socket.socket) -> tuple[int, dict[str, str], bytes]:
    sock.settimeout(15.0)
    data = b""
    while b"\r\n\r\n" not in data:
        chunk = sock.recv(4096)
        if not chunk:
            raise RuntimeError("connection closed before headers were received")
        data += chunk
    status, headers, body = parse_headers(data)
    content_length = int(headers.get("content-length", "0") or "0")
    while len(body) < content_length:
        chunk = sock.recv(content_length - len(body))
        if not chunk:
            raise RuntimeError("connection closed before body was fully received")
        body += chunk
    return status, headers, body


def send_request(sock: socket.socket, request: str) -> tuple[int, dict[str, str], bytes]:
    sock.sendall(request.encode("ascii"))
    return read_response(sock)


def assert_true(name: str, condition: bool, detail: str = "") -> None:
    if condition:
        print(f"[PASS] {name}")
        return
    message = f"[FAIL] {name}"
    if detail:
        message += f": {detail}"
    raise AssertionError(message)


def test_keep_alive() -> None:
    with socket.create_connection((HOST, PORT), timeout=5.0) as sock:
        request = (
            "GET / HTTP/1.1\r\n"
            f"Host: {HOST}\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
        )
        status1, headers1, body1 = send_request(sock, request)
        status2, headers2, body2 = send_request(sock, request)
    assert_true("long connection first request", status1 == 200, f"status={status1}")
    assert_true("long connection second request", status2 == 200, f"status={status2}")
    assert_true("long connection keep-alive header", headers1.get("connection", "").lower() == "keep-alive")
    assert_true("long connection repeated response", body1 == body2 and len(body1) > 0)


def test_connection_timeout() -> None:
    sock = socket.create_connection((HOST, PORT), timeout=5.0)
    try:
        sock.settimeout(IDLE_TIMEOUT + 5)
        time.sleep(IDLE_TIMEOUT + 2)
        try:
            data = sock.recv(1)
        except socket.timeout:
            data = b"timeout"
        assert_true("timeout connection closes", data == b"" , f"recv={data!r}")
    finally:
        sock.close()


def test_bad_request() -> None:
    with socket.create_connection((HOST, PORT), timeout=5.0) as sock:
        sock.sendall(b"BAD / HTTP/1.1\r\nHost: localhost\r\n\r\n")
        sock.settimeout(5.0)
        try:
            status, headers, body = read_response(sock)
            assert_true("error request status", status == 400, f"status={status}")
            assert_true("error request body", len(body) > 0)
        except Exception as exc:  # noqa: BLE001
            raise AssertionError(f"[FAIL] error request did not return a response: {exc}") from exc


def test_business_timeout() -> None:
    with socket.create_connection((HOST, PORT), timeout=5.0) as sock:
        request = (
            "GET /slow HTTP/1.1\r\n"
            f"Host: {HOST}\r\n"
            "Connection: close\r\n"
            "\r\n"
        )
        start = time.time()
        sock.sendall(request.encode("ascii"))
        sock.settimeout(BUSINESS_TIMEOUT + 5)
        status, headers, body = read_response(sock)
        elapsed = time.time() - start
        print(f"[INFO] business timeout response status={status} elapsed={elapsed:.2f}s")
        assert_true("business timeout status", status == 504, f"status={status}")
        assert_true("business timeout latency", elapsed >= BUSINESS_TIMEOUT and elapsed < BUSINESS_TIMEOUT + 3, f"elapsed={elapsed:.2f}")
        assert_true("business timeout body", body != b"", "empty body")


def test_parallel_requests() -> None:
    results: list[tuple[int, int]] = []
    errors: list[Exception] = []

    def worker() -> None:
        try:
            with socket.create_connection((HOST, PORT), timeout=5.0) as sock:
                request = (
                    "GET / HTTP/1.1\r\n"
                    f"Host: {HOST}\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                )
                status, _, body = send_request(sock, request)
                results.append((status, len(body)))
        except Exception as exc:  # noqa: BLE001
            errors.append(exc)

    threads = [threading.Thread(target=worker) for _ in range(20)]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()

    assert_true("parallel requests no errors", not errors, f"errors={errors!r}")
    assert_true("parallel requests all succeeded", all(status == 200 and body_len > 0 for status, body_len in results), f"results={results!r}")


def test_large_file() -> None:
    large_file = ensure_large_file()
    expected = large_file.read_bytes()
    with socket.create_connection((HOST, PORT), timeout=5.0) as sock:
        request = (
            "GET /large.bin HTTP/1.1\r\n"
            f"Host: {HOST}\r\n"
            "Connection: close\r\n"
            "\r\n"
        )
        status, headers, body = send_request(sock, request)
    assert_true("large file status", status == 200, f"status={status}")
    assert_true("large file length", len(body) == len(expected), f"got={len(body)} expected={len(expected)}")
    assert_true("large file checksum", hashlib.sha256(body).hexdigest() == hashlib.sha256(expected).hexdigest())


def test_pressure() -> None:
    total = 200
    concurrency = 20
    latencies: list[float] = []
    errors: list[Exception] = []
    lock = threading.Lock()

    def worker(request_count: int) -> None:
        for _ in range(request_count):
            try:
                start = time.time()
                with socket.create_connection((HOST, PORT), timeout=5.0) as sock:
                    request = (
                        "GET / HTTP/1.1\r\n"
                        f"Host: {HOST}\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                    )
                    status, _, body = send_request(sock, request)
                elapsed = time.time() - start
                if status != 200 or not body:
                    raise RuntimeError(f"unexpected response status={status} body_len={len(body)}")
                with lock:
                    latencies.append(elapsed)
            except Exception as exc:  # noqa: BLE001
                with lock:
                    errors.append(exc)

    per_thread = total // concurrency
    extra = total % concurrency
    threads = []
    start = time.time()
    for index in range(concurrency):
        count = per_thread + (1 if index < extra else 0)
        thread = threading.Thread(target=worker, args=(count,))
        threads.append(thread)
        thread.start()
    for thread in threads:
        thread.join()
    elapsed = time.time() - start

    assert_true("pressure test no errors", not errors, f"errors={errors!r}")
    assert_true("pressure test completed", len(latencies) == total, f"count={len(latencies)} expected={total}")
    rps = total / elapsed if elapsed > 0 else float("inf")
    avg_ms = (sum(latencies) / len(latencies) * 1000.0) if latencies else 0.0
    print(f"[INFO] pressure test: total={total}, concurrency={concurrency}, elapsed={elapsed:.2f}s, rps={rps:.2f}, avg_latency={avg_ms:.2f}ms")


def main() -> int:
    large_file = ensure_large_file()
    if not SERVER_BIN.exists():
        raise SystemExit(f"server binary not found: {SERVER_BIN}")

    server = subprocess.Popen(
        [str(SERVER_BIN)],
        cwd=str(ROOT),
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        start_new_session=True,
    )

    try:
        wait_for_port(HOST, PORT)
        tests = [
            test_keep_alive,
            test_connection_timeout,
            test_bad_request,
            test_business_timeout,
            test_parallel_requests,
            test_large_file,
            test_pressure,
        ]
        failures: list[tuple[str, str]] = []
        for test in tests:
            print(f"[RUN] {test.__name__}")
            try:
                test()
            except Exception as exc:  # noqa: BLE001
                failures.append((test.__name__, str(exc)))
                print(f"[FAIL] {test.__name__}: {exc}")
        if failures:
            print("[SUMMARY] failures:")
            for name, message in failures:
                print(f"[SUMMARY] {name}: {message}")
            return 1
        print("[PASS] all requested scenarios completed")
        return 0
    finally:
        try:
            os.killpg(server.pid, signal.SIGTERM)
        except Exception:  # noqa: BLE001
            server.terminate()
        try:
            server.wait(timeout=5)
        except subprocess.TimeoutExpired:
            try:
                os.killpg(server.pid, signal.SIGKILL)
            except Exception:  # noqa: BLE001
                server.kill()
        try:
            if large_file.exists():
                large_file.unlink()
        except OSError:
            pass


if __name__ == "__main__":
    raise SystemExit(main())