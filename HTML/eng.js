function CalcEconomy()
{
	var fPricekWh    = parseFloat(document.getElementById("PricekWh").value);
	var fTimeIdle    = parseFloat(document.getElementById("TimeIdle").value);
	var fNumberStaff = parseFloat(document.getElementById("NumberStaff").value);  

	var fEconomyMonth = fPricekWh * fTimeIdle / 60 * fNumberStaff * 22 * 0.05;
	var fPrice        = 249.90;

	if (1 == fNumberStaff)
	{
		fPrice = 19.90;
	}
  	
	document.getElementById("EconomyMonth").innerHTML  = '$' + parseInt(fEconomyMonth);
	document.getElementById("EconomyYear").innerHTML   = '$' + parseInt(fEconomyMonth * 11);
	document.getElementById("PaybackPeriod").innerHTML = parseInt(fPrice / fEconomyMonth + 0.5);
}
