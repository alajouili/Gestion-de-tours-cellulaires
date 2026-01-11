import tkinter as tk
import time
import random

WIDTH = 800
HEIGHT = 600
FICHIER_ANTENNES = "Towers.txt"
FICHIER_USERS = "Users.txt"

class TelecomFrontend:
    def __init__(self, root):
        self.root = root
        self.root.title("Visualisation Réseau Telecom - Temps Réel")
        self.canvas = tk.Canvas(root, width=WIDTH, height=HEIGHT, bg="#1e1e1e")
        self.canvas.pack()
        
        self.antenne_positions = {} 
        
        self.status_label = tk.Label(root, text="En attente des données...", fg="white", bg="#333")
        self.status_label.pack(fill=tk.X)

        self.refresh_data()

    def get_position(self, id_antenne):
        if id_antenne not in self.antenne_positions:
            random.seed(id_antenne * 100)
            x = random.randint(50, WIDTH - 50)
            y = random.randint(50, HEIGHT - 50)
            self.antenne_positions[id_antenne] = (x, y)
        return self.antenne_positions[id_antenne]

    def draw_network(self):
        self.canvas.delete("all")
        
        antennes = {} 
        
        try:
            with open(FICHIER_ANTENNES, "r") as f:
                lines = f.readlines()
                if len(lines) > 1:
                    for line in lines[1:]:
                        parts = line.strip().split(';')
                        if len(parts) >= 3:
                            aid = int(parts[0])
                            region = parts[1]
                            x, y = self.get_position(aid)
                            
                            self.canvas.create_polygon(x, y-20, x-15, y+10, x+15, y+10, fill="#00a8ff", outline="white")
                            self.canvas.create_text(x, y+25, text=f"Antenne {aid}\n{region}", fill="white", font=("Arial", 9))
                            
                            self.canvas.create_oval(x-60, y-60, x+60, y+60, outline="#00a8ff", dash=(2, 4))
                            
                            antennes[aid] = (x, y)
        except FileNotFoundError:
            self.canvas.create_text(WIDTH/2, HEIGHT/2, text="Fichier Towers.txt introuvable", fill="red")
            return

        user_count = 0
        try:
            with open(FICHIER_USERS, "r") as f:
                for line in f:
                    parts = line.strip().split(';')
                    if len(parts) >= 5:
                        aid = int(parts[0])
                        uid = int(parts[1])
                        nom = parts[2]
                        
                        if aid in antennes:
                            ax, ay = antennes[aid]
                            
                            random.seed(uid) 
                            ux = ax + random.randint(-40, 40)
                            uy = ay + random.randint(-40, 40)
                            
                            credit = float(parts[4])
                            color = "#00ff00" if credit > 0 else "#ff0000"
                            
                            self.canvas.create_oval(ux-5, uy-5, ux+5, uy+5, fill=color, outline="white")
                            self.canvas.create_text(ux, uy-15, text=nom, fill="#cccccc", font=("Arial", 8))
                            
                            self.canvas.create_line(ax, ay+5, ux, uy, fill="#444", width=1)
                            user_count += 1
                            
        except FileNotFoundError:
            pass

        self.status_label.config(text=f"Système actif | Antennes : {len(antennes)} | Utilisateurs connectés : {user_count}")

    def refresh_data(self):
        self.draw_network()
        self.root.after(1000, self.refresh_data)

if __name__ == "__main__":
    root = tk.Tk()
    app = TelecomFrontend(root)
    root.mainloop()