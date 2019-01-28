#!/usr/bin/bash

cd src

CSS=`npx uglifycss style.css`

JS=`npx -q uglifyjs rgb.js --ecma 8 --rename --toplevel -c -m`

HTML=`sed -e "s#__JAVASCRIPT__#$JS#" -e "s@__CSS__@$CSS@" -e "s#__JAVASCRIPT__#\&#" template.html`

npx html-minifier --collapse-whitespace --minify-css --minify-js --remove-empty-elements --use-short-doctype --sort-attributes --sort-class-name -o index.html <<< "$HTML"

# Replace hsv.png with base64 uri
npx adauru-cli r > /dev/null

mv build/*.html ../index.html

rmdir build