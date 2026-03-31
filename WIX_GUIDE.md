# How to Edit Your Wix Website — Modern & Luxury Style

This guide explains how to apply the modern luxury design (defined in [`index.html`](./index.html))
to your Wix website using the **Wix Editor** and **Wix Studio**.

---

## Important: What GitHub Can and Cannot Do With Wix

Wix is a **closed, proprietary platform** — your site lives on Wix's servers, not in this
repository. That means:

| Action | Possible via GitHub? | How to do it |
|---|---|---|
| Edit pages, layout, colors, text | ❌ No | Use Wix Editor (see below) |
| Add custom CSS / HTML widgets | ✅ Partially | Paste code into Wix HTML element |
| Deploy a replacement static site | ✅ Yes | Use GitHub Pages (see below) |
| Version-control your Wix Velo code | ✅ Yes | Wix → Velo → GitHub sync |

Use this repo's [`index.html`](./index.html) as a **visual reference / blueprint** while
you work inside the Wix Editor.

---

## Option 1 — Edit Directly in Wix Editor (Recommended)

### Step 1 · Open Your Site

1. Go to [manage.wix.com](https://manage.wix.com) and log in.
2. Click **Edit Site** next to your website.
3. The **Wix Editor** opens in your browser.

---

### Step 2 · Apply the Luxury Color Palette

The luxury design uses these exact colors — add them to your Wix site palette:

| Role | Hex Value | Where to use |
|---|---|---|
| Background | `#0a0a0a` | Page background, section fills |
| Dark surface | `#111111` | Card and section backgrounds |
| Gold accent | `#c9a84c` | Buttons, dividers, highlights |
| Gold light | `#e8d5a3` | Hover states |
| Body text | `#f5f0ea` | All paragraph text |
| Muted text | `rgba(245,240,234,0.7)` | Secondary / caption text |

**To set site colors in Wix Editor:**
1. Click the **Design** panel on the left (paint-bucket icon).
2. Select **Site Design → Color Palette**.
3. Replace the palette colors with the hex values above.
4. Click **Apply to Site** — Wix will update all themed elements automatically.

---

### Step 3 · Set Luxury Fonts

| Role | Font | Weight |
|---|---|---|
| Headings (H1–H3) | **Cormorant Garamond** | Light (300) / Regular (400) |
| Body / Nav / Buttons | **Montserrat** | Light (300) / Medium (500) |

**To change fonts in Wix Editor:**
1. Click **Design → Site Design → Text Themes**.
2. For each heading level, click the font name and search `Cormorant Garamond`.
3. For body text, search `Montserrat`.
4. Save the theme — all text elements using theme fonts update instantly.

---

### Step 4 · Rebuild Each Section

Use the structure in [`index.html`](./index.html) as your layout reference.

#### Hero Section
1. Add a **full-height strip** (right-click canvas → Add Section → Blank).
2. Set the strip background to `#0a0a0a`.
3. Add a **Text** element → H1 size, Cormorant Garamond, color `#f5f0ea`.
   - Make the key word italic and color it `#c9a84c`.
4. Add a **small text** element above the H1 for the tagline (Montserrat, `#c9a84c`, letter-spacing 6px).
5. Add two **Button** elements:
   - Primary: background `#c9a84c`, text color `#0a0a0a`, no border radius.
   - Outline: transparent background, border `1px solid #c9a84c`, text color `#c9a84c`.
6. Optional: Add a **Video / Parallax background** for depth.

#### About Section
1. Add a two-column strip.
2. Left column: insert a decorative **image** or **box** with a gold border (`#c9a84c`, 1px).
3. Right column: section label (Montserrat 11px, gold, letter-spacing 5px), H2, paragraph, and a
   stats row (3 columns, large Cormorant Garamond numbers in gold).

#### Services Section
1. Add a **3-column repeater** (Add Elements → Repeater).
2. In each card: a large faded number, a small SVG icon, H3 title, body paragraph.
3. Set card background to `#111111` with a `2px solid transparent` top border that turns gold on hover
   (use Wix Interactions → Hover → change border color).

#### Portfolio Section
1. Use a **Wix Pro Gallery** (Add Elements → Gallery → Grid Gallery).
2. Upload project images or use placeholder images.
3. In Gallery Settings → Design, set the overlay to dark with text below.

#### Testimonials Section
1. Add a **3-column repeater**.
2. Each card: large `"` character (Cormorant Garamond, gold), star rating (★★★★★), quote text
   (italic), author avatar (circle image), author name + title.

#### Contact Section
1. Add a **Wix Forms** element (Add Elements → Contact & Forms → Contact Form).
2. Style fields: background `rgba(255,255,255,0.04)`, border `1px solid rgba(201,168,76,0.2)`.
3. Submit button: background `#c9a84c`, text `#0a0a0a`, no border radius.

---

### Step 5 · Add a Custom HTML Widget (Advanced)

If you want to copy sections of the CSS directly into Wix:

1. In Wix Editor, click **Add Elements (+) → Embed → HTML iFrame**.
2. Click the iFrame element → **Enter Code**.
3. Paste a self-contained HTML snippet (a single card or section from `index.html`).
4. Click **Apply** — the element renders live on your page.

> ⚠️ iFrame elements do not inherit Wix fonts or colors, so include your CSS inside the `<style>` tag
> within the pasted snippet.

---

### Step 6 · Publish

1. Click the **Publish** button (top-right) in Wix Editor.
2. Your changes go live on your Wix domain instantly.

---

## Option 2 — Deploy the Reference Site via GitHub Pages

If you want to **host the `index.html` luxury site directly** (not through Wix):

1. In your GitHub repository, go to **Settings → Pages**.
2. Under **Source**, select **Deploy from a branch**.
3. Choose branch `main` (or your default branch) and folder `/root`.
4. Click **Save** — GitHub will publish `index.html` at:
   ```
   https://eleazarcariaga92-hue.github.io/ideal-pancake/
   ```
5. You can then point a custom domain to this URL in **Settings → Pages → Custom domain**.

---

## Option 3 — Sync Wix Velo Code with GitHub

If your Wix site uses **Wix Velo** (JavaScript for dynamic functionality):

1. In Wix Editor, enable **Dev Mode** (top bar → Dev Mode → Turn on Dev Mode).
2. In the Velo sidebar, click **GitHub** → **Connect to GitHub**.
3. Authorize Wix to access your repository.
4. Select the `ideal-pancake` repo (or create a new one).
5. From this point, commits to the connected branch are synced to your Wix site's Velo code.

> Note: Velo sync covers **JavaScript logic only** — layout and design must still be edited
> in the Wix Editor.

---

## Quick Reference — Luxury Design Tokens

```css
/* Paste these into any Wix Custom CSS field or HTML widget */
:root {
  --gold:       #c9a84c;
  --gold-light: #e8d5a3;
  --black:      #0a0a0a;
  --dark:       #111111;
  --white:      #f5f0ea;

  --font-heading: 'Cormorant Garamond', serif;
  --font-body:    'Montserrat', sans-serif;
}
```

---

## Need Help?

- [Wix Editor official guide](https://support.wix.com/en/article/wix-editor-getting-started)
- [Wix Studio documentation](https://support.wix.com/en/wix-studio)
- [Wix Velo GitHub integration](https://support.wix.com/en/article/velo-working-with-github)
- Design reference: [`index.html`](./index.html) in this repository
