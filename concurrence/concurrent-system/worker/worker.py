import threading
import psycopg2
import time
import random
import os

DB_CONFIG = {
    "host": "db",
    "database": "testdb",
    "user": "user",
    "password": "password"
}

semaphore = threading.Semaphore(3)
file_lock = threading.Lock()

SHARED_FILE = "/shared/logs.txt"

worker_id = os.getenv("WORKER_ID", "worker")


def get_job(conn):
    cur = conn.cursor()

    cur.execute("""
        SELECT id, description
        FROM input
        WHERE status = 'pending'
        FOR UPDATE SKIP LOCKED
        LIMIT 1;
    """)

    row = cur.fetchone()

    if row:
        cur.execute(
            "UPDATE input SET status='in_process' WHERE id=%s",
            (row[0],)
        )

    return row


def process_job():
    while True:
        with semaphore:
            conn = psycopg2.connect(**DB_CONFIG)
            conn.autocommit = False

            try:
                job = get_job(conn)

                if not job:
                    print(f"{worker_id} sin trabajo, reintentando...")
                    conn.commit()
                    conn.close()
                    time.sleep(2)
                    continue
                
                input_id, description = job

                print(f"{worker_id} procesando {input_id}")

                time.sleep(random.uniform(0.5, 2))

                result = description.upper()

                save_result(conn, input_id, result)
                mark_done(conn, input_id)
                write_log(input_id)

                conn.commit()

            except Exception as e:
                conn.rollback()
                print("Error:", e)

            finally:
                conn.close()


def save_result(conn, input_id, result):
    cur = conn.cursor()
    cur.execute("""
        INSERT INTO result (input_id, worker_identifier, result)
        VALUES (%s, %s, %s)
    """, (input_id, worker_id, result))


def mark_done(conn, input_id):
    cur = conn.cursor()
    cur.execute(
        "UPDATE input SET status='processed' WHERE id=%s",
        (input_id,)
    )


def write_log(input_id):
    with file_lock:
        with open(SHARED_FILE, "a") as f:
            f.write(f"{worker_id} procesó {input_id} en {time.time()}\n")


def wait_for_db():
    while True:
        try:
            conn = psycopg2.connect(**DB_CONFIG)
            conn.close()
            print("DB lista")
            break
        except:
            print("Esperando DB...")
            time.sleep(2)

def main():
    wait_for_db()

    while True:
        threads = []

        for _ in range(3):
            t = threading.Thread(target=process_job)
            t.start()
            threads.append(t)

        for t in threads:
            t.join()

        print(f"{worker_id} reiniciando ciclo...")
        time.sleep(2)


if __name__ == "__main__":
    main()

