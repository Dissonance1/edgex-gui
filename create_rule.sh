#!/bin/bash
curl -s -X POST http://localhost:9081/rules -H "Content-Type: application/json" -d @"${1:-final_notification_rule.json}"
