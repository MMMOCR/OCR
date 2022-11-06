echo "#### Adding remote"
git remote add mirror "$REMOTE_URL"
echo "#### Remote added"
git push mirror
echo "#### Pushed to mirror"
