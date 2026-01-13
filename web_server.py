from flask import Flask, render_template_string, request
import os

app = Flask(__name__)

HTML = """
<!DOCTYPE html>
<html>
<head>
    <title>Graph Engine Control</title>
    <style>
        body { background: #1a1a1a; color: white; font-family: sans-serif; text-align: center; padding-top: 50px; }
        .card { background: #2a2a2a; padding: 30px; border-radius: 15px; display: inline-block; box-shadow: 0 4px 15px rgba(0,0,0,0.5); }
        input { padding: 12px; border-radius: 5px; border: none; width: 200px; }
        button { padding: 12px 25px; background: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer; font-weight: bold; }
        button:hover { background: #0056b3; }
        .status { margin-top: 20px; color: #28a745; font-weight: bold; }
    </style>
</head>
<body>
    <div class="card">
        <h1>��️ Graph Engine Dashboard</h1>
        <form action="/add" method="get">
            <input type="text" name="label" placeholder="Enter Node Name..." required>
            <button type="submit">Add Node</button>
        </form>
        {% if msg %}
            <p class="status">{{ msg }}</p>
        {% endif %}
    </div>
</body>
</html>
"""

@app.route('/')
def home():
    return render_template_string(HTML)

@app.route('/add')
def add_node():
    label = request.args.get('label')
    try:
        with open("graph_pipe", "w") as pipe:
            pipe.write(f"add {label}\\n")
        return render_template_string(HTML, msg=f"✅ Sent 'add {label}' to Engine!")
    except Exception as e:
        return render_template_string(HTML, msg=f"❌ Error: {str(e)}")

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=18080)
