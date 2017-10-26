# Import a library of functions called 'pygame'
import pygame
 
# Initialize the game engine
pygame.init()
 
# Define some colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
GREEN = (0, 255, 0)
RED = (255, 0, 0)
 
PI = 3.141592653
 
# Set the height and width of the screen
size = (600, 400)
screen = pygame.display.set_mode(size)
 
# Loop until the user clicks the close button.
done = False
clock = pygame.time.Clock()

# Listof point
pointDragon = [
    [65, 17],
    [267, 43],
    [299, 95],
    [348, 107],
    [347, 104],
    [359, 116],
    [360, 111],
    [375, 122],
    [376, 115],
    [386, 130],
    [398, 129],
    [409, 146],
    [407, 148],
    [426, 160],
    [415, 168],
    [401, 154],
    [383, 146],
    [383, 136],
    [313, 111],
    [299, 110],
    [279, 121],
    [264, 139],
    [260, 163],
    [270, 185],
    [298, 201],
    [384, 217],
    [414, 229],
    [431, 241],
    [450, 265],
    [457, 290],
    [445, 266],
    [423, 243],
    [393, 227],
    [375, 225],
    [401, 246],
    [423, 278],
    [436, 310],
    [439, 335],
    [425, 296],
    [402, 260],
    [367, 231],
    [341, 221],
    [297, 215],
    [273, 205],
    [254, 185],
    [247, 174],
    [245, 153],
    [254, 126],
    [279, 104],
    [268, 79],
    [228, 81],
    [161, 100],
    [101, 127],
    [100, 124],
    [174, 90],
    [230, 74],
    [264, 70],
    [263, 64],
    [147, 64],
    [61, 75],
    [158, 58],
    [262, 54],
    [208, 35],
    [130, 22],
    [64, 22],
    [65, 18],
    [167, 22],
    [219, 29],
    [252, 39]
    ]
    
    
# Loop as long as done == False
while not done:
 
    for event in pygame.event.get():  # User did something
        if event.type == pygame.QUIT:  # If user clicked close
            done = True  # Flag that we are done so we exit this loop
 
    # All drawing code happens after the for loop and but
    # inside the main while not done loop.
 
    # Clear the screen and set the screen background
    screen.fill(RED)

    #draw dragon
    pygame.draw.polygon(screen, WHITE, pointDragon, 0)
    '''pygame.draw.line(screen, WHITE, [82, 14], [291, 44], 1)
    pygame.draw.line(screen, WHITE, [82, 14], [278, 47], 1)
    pygame.draw.line(screen, WHITE, [79, 69], [278, 47], 1)
    pygame.draw.line(screen, WHITE, [79, 69], [279, 60], 1)
    pygame.draw.line(screen, WHITE, [115, 120], [279, 60], 1)
    pygame.draw.line(screen, WHITE, [115, 120], [286, 76], 1)
    pygame.draw.line(screen, WHITE, [286, 76], [297, 97], 1)
    pygame.draw.arc(screen, WHITE, [259, 97, (369-259), (204-97)], PI/1.7, 3*PI/2, 1)
    pygame.draw.line(screen, WHITE, [314, 202], [397, 219], 1)
    pygame.draw.line(screen, WHITE, [397, 219], [496, 339], 1)
    pygame.draw.line(screen, WHITE, [496, 339], [440, 240], 1)
    pygame.draw.line(screen, WHITE, [440, 240], [550, 300], 1)
    pygame.draw.line(screen, WHITE, [550, 300], [420, 195], 1)
    pygame.draw.arc(screen, WHITE, [320, 97, 2*(420-315), (200-97)], PI/1.2, 3*PI/2, 1)
    pygame.draw.line(screen, WHITE, [335, 125], [315, 76], 1)'''

    # Go ahead and update the screen with what we've drawn.
    # This MUST happen after all the other drawing commands.
    pygame.display.flip()
 
    # This limits the while loop to a max of 60 times per second.
    # Leave this out and we will use all CPU we can.
    clock.tick(60)
 
# Be IDLE friendly
pygame.quit()

