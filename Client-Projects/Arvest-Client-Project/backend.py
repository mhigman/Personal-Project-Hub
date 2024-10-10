from flask import Flask, render_template, request, jsonify
import json
# TODO Create core living budget form
# TODO Save json for multiple financial accounts, 
# allowing for a dropdown to select between them.
# TODO Post to arvest "account" for "withdrawl".



# This is just an exapmle of data stored about a user
# This could be any data 
# from an authenticated user from you database


app = Flask(__name__)

@app.route("/", methods=["GET"])
def index():
    return render_template("index.html")
    

@app.route("/core-budget", methods=["GET"])
def core_living_budget_form():
    return render_template("core_budget_form.html")




if __name__ == "__main__":
    app.run(debug=True)