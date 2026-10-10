#!/usr/bin/env python3
"""Convert an official Riftbound rules PDF into rules/*.md.

The Core Rules and Tournament Rules PDFs are linked from the Rules Hub
(https://playriftbound.com/en-us/rules-hub/). Rule numbers sit in a fixed left
column, so each numbered rule becomes one markdown line (nested by depth);
section titles are recovered from font size and bold/italic from the font name.

Requires PyMuPDF (`pip install pymupdf`), which is NOT an engine dependency.

Usage:
    python3 scripts/rules_to_md.py rules/core-rules.pdf rules/core-rules.md "Riftbound Core Rules"
    python3 scripts/rules_to_md.py rules/tournament-rules.pdf rules/tournament-rules.md "Riftbound Tournament Rules"
"""
import re, sys, pymupdf
src, dst, title = sys.argv[1], sys.argv[2], sys.argv[3]
doc = pymupdf.open(src)
NUM = re.compile(r"^\d{3}(?:\.[0-9a-z]+)*\.$")
def fmt(spans):
    out = ""
    for s in spans:
        t = s["text"]
        if not t.strip():
            out += t; continue
        f = s["font"]; core = t.strip(); lead = t[:len(t)-len(t.lstrip())]; trail = t[len(t.rstrip()):]
        if "BoldItalic" in f: core = f"***{core}***"
        elif "Bold" in f and s["size"] < 10: core = f"**{core}**"
        elif "Italic" in f: core = f"*{core}*"
        out += lead + core + trail
    out = re.sub(r"\*\*\s*\*\*", " ", out); out = re.sub(r"(?<!\*)\*\s+\*(?!\*)", " ", out)
    return re.sub(r"\s+", " ", out).strip()
items = []   # (kind, number, text, size)
cur = None; updated = None
for page in doc:
    for b in page.get_text("dict")["blocks"]:
        for l in b.get("lines", []):
            spans = [s for s in l["spans"]]
            raw = "".join(s["text"] for s in spans).strip()
            if not raw: continue
            size = max(s["size"] for s in spans if s["text"].strip())
            m = re.match(r"Last Updated:?\s*(.+)", raw)
            if m and not updated: updated = m.group(1).strip(); continue
            if size >= 29: continue  # document title
            first = spans[0]["text"].strip()
            head = raw.split(" ", 1)[0]
            if NUM.match(head) and l["bbox"][0] < 60:
                rest = fmt(spans)[len(head):].strip() if fmt(spans).startswith(head) else fmt(spans).split(" ",1)[-1]
                rest = re.sub(r"^\*+\s*", lambda mm: mm.group(0).strip(), rest)
                cur = {"num": head, "text": rest, "size": size, "ex": []}
                items.append(cur); continue
            if cur is None: continue
            text = fmt(spans)
            if re.match(r"^\*?(Example|See)\b", text.replace("*", "")[:8] and text.lstrip("*")):
                cur["ex"].append(text)
            elif cur["ex"]:
                cur["ex"][-1] += " " + text
            else:
                cur["text"] = (cur["text"] + " " + text).strip()
                cur["size"] = max(cur["size"], size)
out = [f"# {title}", "", f"Last Updated: {updated}", "", "---", ""]
for it in items:
    num, text = it["num"], it["text"].replace("**", "") if it["size"] >= 10 else it["text"]
    text = re.sub(r"^\* ", "", text)
    depth = num.count(".") - 1
    if it["size"] >= 20: out += ["", f"# {num} {text}", ""]
    elif it["size"] >= 10: out += ["", f"## {num} {text}", ""]
    elif depth == 0: out += ["", f"{num} {text}"]
    else: out += [f"{'   ' * depth}- {num} {text}"]
    for e in it["ex"]:
        out += [f"{'   ' * (depth + 1)}> {e}"]
open(dst, "w").write(re.sub(r"\n{3,}", "\n\n", "\n".join(out)) + "\n")
print(len(items), "numbered rules ->", dst, "| updated", updated)
