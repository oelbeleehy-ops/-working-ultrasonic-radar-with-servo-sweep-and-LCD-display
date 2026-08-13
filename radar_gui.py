import serial
import pygame
import math
import sys

portName ="COM3"
baudRate = 9600
width = 1200
height = 700
centre_X = width / 2
centre_Y = height -45
max_range = 250
radar_radius = min(width// 2 -40,centre_Y - 80)
green = (0, 255, 0)
black = (0, 0, 0)
red = (255, 0, 0)
dgreen = (0, 100, 0)
white = (255, 255, 255)
grey = (120, 120, 120)

def main():
    try:
        ser = serial.Serial(portName, baudRate, timeout=1)
    except serial.SerialException as e:
        print(f"Error opening {portName}: {e}")
        print("Please check the port name and ensure the device is connected.")
        sys.exit(1)
    pygame.init()
    screen = pygame.display.set_mode((width, height))
    pygame.display.set_caption("Ultrasonic Radar Visualization")
    font = pygame.font.SysFont("consolas", 22)
    small_font = pygame.font.SysFont("consolas", 16)
    title_font = pygame.font.SysFont("consolas", 26, bold=True)
    clock = pygame.time.Clock()
    angle = 90
    distance = 0
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
        if ser.in_waiting:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line.endswith("."):
                    line = line[:-1]
                if "," in line:
                    parts = line.split(",")
                    angle = int(parts[0])
                    distance = float(parts[1])
                    angle = max(0, min(180, angle))
                    distance = max(0, distance)
            except (ValueError, IndexError) as e:
                print(f"PARSE ERROR: {e} on line: {repr(line)}")
        screen.fill(black)
        draw_radar_background(screen)
        draw_sweep_line(screen, angle)
        draw_blip(screen, angle, distance)
        draw_text(screen,title_font, font,small_font, angle, distance)
        pygame.display.flip()
        clock.tick(60)
    ser.close()
    pygame.quit()

def draw_radar_background(screen):
    for r in range(1,5):
        radius = radar_radius * r / 4
        rect = pygame.Rect(centre_X - radius, centre_Y - radius, 2 * radius, 2 * radius)
        pygame.draw.arc(screen, dgreen, rect, math.pi, 2*math.pi, 2)

    for angle in range(0, 181, 30):
        rad = math.radians(angle)
        x = centre_X + radar_radius * math.cos(rad)
        y = centre_Y - radar_radius * math.sin(rad)
        pygame.draw.line(screen, dgreen, (centre_X, centre_Y), (x, y), 1)
    pygame.draw.line(screen, dgreen, (centre_X - radar_radius, centre_Y), (centre_X + radar_radius, centre_Y), 2)
    for r in range(1,5):
        distance_value = (max_range * r) / 4
        radius = radar_radius * r / 4
        text = pygame.font.SysFont("consolas", 14).render(f"{int(distance_value)} cm", True, dgreen)
        screen.blit(text, (centre_X + 8, centre_Y - radius - 8))
def draw_sweep_line(screen, angle):
    rad = math.radians(angle)
    x = centre_X + radar_radius * math.cos(rad)
    y = centre_Y - radar_radius * math.sin(rad)
    pygame.draw.line(screen, green, (centre_X, centre_Y), (x, y), 3)
def draw_blip(screen, angle, distance):
    if 0 < distance <= max_range:
        rad = math.radians(angle)
        r =(distance / max_range) * radar_radius
        x = centre_X + r * math.cos(rad)
        y = centre_Y - r * math.sin(rad)
        pygame.draw.circle(screen, red, (int(x), int(y)), 7)
def draw_centre_point(screen):
    pygame.draw.circle(screen, red, (centre_X, centre_Y), 7)
def draw_text(screen,title_font, font,small_font, angle, distance):
    title = title_font.render("Ultrasonic Radar Visualization", True, green)
    screen.blit(title,(20,18))
    angle_text = font.render(f"Angle: {angle}°", True, green)
    if distance > 0:
        distance_text = font.render(f"Distance: {distance:.1f} cm", True, green)
    else:
        distance_text = font.render("Distance: --- ", True, green)
    screen.blit(distance_text, (20, 92))
    range_text = small_font.render(f"Range: 0 - {max_range} cm", True, dgreen)
    screen.blit(range_text, (20, 125))
if __name__ == "__main__":
    main()  
                
