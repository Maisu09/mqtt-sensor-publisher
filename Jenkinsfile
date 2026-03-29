pipeline{
    agent any
    environment{
        COMPOSE_FILE = 'docker-comnpose.docker.yaml'
    }
    stages{
        stage('Pre-check Cleaning'){
            steps{
                step('compose_down') {
                    sh 'docker compose down --remove-orphans || true'
                }
                step('remove_existing_images') {
                    sh 'docker compose down --rmi all || true'
                }
            }
        }
        stage('Checkout'){
            steps {
                checkout scm
            }
        }
        stage('Verify Compose File') {
            steps{
                sh 'docker compose config'
            }
        }
        stage('Build Images') {
            steps{
                sh 'docker compose build'
            }
        }
        stage('Deploy') {
            steps{
                sh 'docker compose up -d --remove-orphans || true'
            }
        }
    }

    post{
        success {
            echo 'Deployment successful!'
        }
        failure {
            echo 'Pipeline failed - check logs'
            sh 'docker compose down || true'
        }
    }
}