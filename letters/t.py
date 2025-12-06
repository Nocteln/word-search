import os
import random
import string
from PIL import Image, ImageDraw, ImageFont, ImageOps, ImageFilter

# --- CONFIGURATION ---
OUTPUT_DIR = "dataset_ocr_var_size"
CANVAS_SIZE = (64, 64)      # Taille finale de l'image (ne change pas)
SAMPLES_PER_LETTER = 50

# Paramètres de Taille
MIN_SIZE = 20               # Taille minimum de la lettre (en pixels)
MAX_SIZE = 60               # Taille maximum (max 64)

# Paramètres de style
ROTATION_MAX = 10           # Rotation un peu plus forte permise (+/- 10°)

# Paramètres de dégradation (réduits pour cet exercice)
GRAIN_STRENGTH = 0.10       
GRAIN_CHANCE = 0.5          
BLUR_CHANCE = 0.3           
BLUR_RADIUS_MAX = 1.0       

def get_best_font():
    """Cherche une police système épaisse."""
    candidates = ["arialbd.ttf", "Arial_Bold.ttf", "arial.ttf", "tahoma.ttf", 
                  "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"]
    for font_name in candidates:
        try:
            # On charge en très gros pour avoir une source de qualité
            return ImageFont.truetype(font_name, 120)
        except OSError:
            continue
    print("ATTENTION: Police par défaut utilisée.")
    return ImageFont.load_default()

def add_grain(img, strength):
    if strength <= 0: return img
    noise_data = os.urandom(img.width * img.height)
    noise_img = Image.frombytes('L', img.size, noise_data)
    return Image.blend(img, noise_img, alpha=strength)

def create_dataset():
    if not os.path.exists(OUTPUT_DIR): os.makedirs(OUTPUT_DIR)
    font = get_best_font()
    letters = string.ascii_uppercase
    global_counter = 0

    print(f"Génération dans '{OUTPUT_DIR}'...")
    print(f"Tailles variables entre {MIN_SIZE}px et {MAX_SIZE}px sur fond 64x64.")

    for char in letters:
        for _ in range(SAMPLES_PER_LETTER):
            # --- ÉTAPE 1 : Création haute qualité ---
            # On dessine la lettre en très grand sur un grand canvas temporaire
            large_dim = 300
            img_temp = Image.new('L', (large_dim, large_dim), color=255)
            draw = ImageDraw.Draw(img_temp)
            
            # Centrage approximatif sur le grand canvas
            bbox = draw.textbbox((0, 0), char, font=font)
            w_txt, h_txt = bbox[2]-bbox[0], bbox[3]-bbox[1]
            draw.text(((large_dim-w_txt)/2, (large_dim-h_txt)/2), char, font=font, fill=0)

            # Rotation
            if ROTATION_MAX > 0:
                angle = random.uniform(-ROTATION_MAX, ROTATION_MAX)
                img_temp = img_temp.rotate(angle, resample=Image.BICUBIC, fillcolor=255)

            # --- ÉTAPE 2 : Isolation et Redimensionnement ---
            # On découpe (crop) exactement autour de la lettre
            inverted_img = ImageOps.invert(img_temp)
            bbox = inverted_img.getbbox()
            
            if bbox:
                cropped_letter = img_temp.crop(bbox)
                w_crop, h_crop = cropped_letter.size
                
                # C'EST ICI QUE TOUT CHANGE :
                # 1. On choisit une taille cible aléatoire pour la plus grande dimension
                target_size = random.randint(MIN_SIZE, MAX_SIZE)
                
                # 2. Calcul du ratio pour atteindre cette taille
                ratio = target_size / max(w_crop, h_crop)
                new_w = int(w_crop * ratio)
                new_h = int(h_crop * ratio)
                
                # 3. Redimensionnement de la lettre isolée
                if new_w > 0 and new_h > 0:
                    resized_letter = cropped_letter.resize((new_w, new_h), resample=Image.LANCZOS)
                    
                    # --- ÉTAPE 3 : Placement sur le canvas final 64x64 ---
                    final_img = Image.new('L', CANVAS_SIZE, color=255)
                    
                    # Calcul des positions possibles pour ne pas sortir du cadre
                    max_x_pos = CANVAS_SIZE[0] - new_w
                    max_y_pos = CANVAS_SIZE[1] - new_h
                    
                    # Choix d'une position aléatoire (si de la place existe)
                    paste_x = random.randint(0, max_x_pos) if max_x_pos > 0 else 0
                    paste_y = random.randint(0, max_y_pos) if max_y_pos > 0 else 0
                    
                    # On colle la lettre
                    final_img.paste(resized_letter, (paste_x, paste_y))
                    
                    # --- ÉTAPE 4 : Dégradations finales ---
                    if random.random() < BLUR_CHANCE:
                        radius = random.uniform(0.5, BLUR_RADIUS_MAX)
                        final_img = final_img.filter(ImageFilter.GaussianBlur(radius))

                    if random.random() < GRAIN_CHANCE:
                        final_img = add_grain(final_img, strength=GRAIN_STRENGTH)
                    
                    # Sauvegarde
                    filename = f"{global_counter}{char.lower()}.png"
                    final_img.save(os.path.join(OUTPUT_DIR, filename))
                    global_counter += 1

    print(f"Terminé ! {global_counter} images générées.")

if __name__ == "__main__":
    create_dataset()
