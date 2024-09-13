req.session.inputData = {
    hasError: true,
    message: 'Invalid input - please check your data.',
    email: enteredEmail,
    confirmEmail: enteredConfirmEmail,
    password: enteredPassword,
  };

  module.export = {}