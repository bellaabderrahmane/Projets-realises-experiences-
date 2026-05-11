#!/bin/bash

TPCAS="./bin/tpcc"
TESTS_CORRECTS="./test/good"
TESTS_INCORRECTS="./test/syn-err"
TESTS_SEMERREUR="./test/sem-err"
TESTS_WARNING="./test/warn"
# Compteurs pour les résultats des tests
total_tests=0
passed_tests=0

# Fonction pour exécuter un test
run_test() {
    local file="$1"
    local expected_result="$2"

    echo "Test sur le fichier $file"
    # Capture de la sortie standard et d'erreur du programme
    output=$($TPCAS < "$file" 2>&1)
    local result=$?

    if [ "$result" -eq 0 ]; then
        if [ "$expected_result" -eq 0 ]; then
            echo "  Réussi"
            ((passed_tests++))
        else
            echo "  Échec (erreur non attendue)"
        fi
    else
        if echo "$output" | grep -q "erreur sémantique"; then
            if [ "$expected_result" -ne 0 ]; then
                echo "  Réussi (erreur sémantique attendue)"
                ((passed_tests++))
            else
                echo "  Échec (erreur sémantique non attendue)"
            fi
        else
            if [ "$expected_result" -ne 0 ]; then
                echo "  Réussi (autre erreur attendue)"
                ((passed_tests++))
            else
                echo "  Échec (autre erreur non attendue)"
            fi
        fi
    fi
    ((total_tests++))
}

# Exécution des tests corrects
echo "Tests des programmes corrects :"
for test_file in $TESTS_CORRECTS/*.tpc; do
    run_test "$test_file" 0
done

echo "Tests des programmes qui contient des warning:"
for test_file in $TESTS_WARNING/*.tpc; do
    run_test "$test_file" 1
done

# Exécution des tests incorrects
echo "Tests des programmes incorrects :"
for test_file in $TESTS_INCORRECTS/*.tpc; do
    run_test "$test_file" 1
done

echo "Tests des programmes semantiquement incorrects :"
for test_file in $TESTS_SEMERREUR/*.tpc; do
    run_test "$test_file" 2
done

# Affichage des résultats
echo "Résultats des tests : $passed_tests/$total_tests réussis"
