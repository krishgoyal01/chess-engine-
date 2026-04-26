from PIL import Image, ImageDraw, ImageFont

# Create image
img = Image.new('RGBA', (1200, 400), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)

# Load font (needs a font that supports chess Unicode characters)
font = ImageFont.truetype("seguisym.ttf", 160)

# Chess pieces in order: King, Queen, Rook, Bishop, Knight, Pawn
pieces_white = "♔♕♖♗♘♙"
pieces_black = "♚♛♜♝♞♟"

# Draw white pieces
for i, piece in enumerate(pieces_white):
    draw.text((i*200 + 20, 20), piece, font=font, fill=(255, 255, 255))

# Draw black pieces
for i, piece in enumerate(pieces_black):
    draw.text((i*200 + 20, 220), piece, font=font, fill=(0, 0, 0))

# Save image
img.save('chess_pieces.png')