#!/bin/bash
curl -s -X POST http://localhost:9081/rules -H "Content-Type: application/json" -d @/home/aetina/Desktop/debug_rule.json
